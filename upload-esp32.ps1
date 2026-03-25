param(
  [string]$Port,
  [switch]$SkipCompile,
  [switch]$WaitForDevice,
  [int]$WaitTimeoutSeconds = 30,
  [string]$CliPath = 'C:\Program Files\Arduino CLI\arduino-cli.exe',
  [string]$SketchPath = (Join-Path $PSScriptRoot 'vmix_tally_esp32'),
  [string]$Fqbn = 'esp32:esp32:esp32'
)

if (-not (Test-Path $CliPath)) {
  throw "arduino-cli introuvable: $CliPath"
}

if (-not (Test-Path $SketchPath)) {
  throw "Sketch introuvable: $SketchPath"
}

function Get-SerialPorts {
  param(
    [string]$ArduinoCliPath
  )

  $raw = & $ArduinoCliPath board list --format json

  if ($LASTEXITCODE -ne 0) {
    throw 'Impossible de lister les ports serie avec arduino-cli.'
  }

  $parsed = $raw | ConvertFrom-Json
  $ports = @()

  foreach ($entry in ($parsed.detected_ports | Where-Object { $_ -and $_.port })) {
    $address = $entry.port.address
    $protocol = $entry.port.protocol

    if ($protocol -eq 'serial' -and $address -match '^COM\d+$') {
      $ports += $address.ToUpperInvariant()
    }
  }

  return @($ports | Sort-Object -Unique)
}

function Wait-ForNewSerialPort {
  param(
    [string[]]$ExistingPorts,
    [string]$ArduinoCliPath,
    [int]$TimeoutSeconds
  )

  $baseline = @($ExistingPorts | ForEach-Object { $_.ToUpperInvariant() } | Sort-Object -Unique)
  $deadline = (Get-Date).AddSeconds($TimeoutSeconds)

  if ($baseline.Count -gt 0) {
    Write-Host "Ports detectes actuellement: $($baseline -join ', ')"
  } else {
    Write-Host 'Aucun port serie detecte actuellement.'
  }

  Write-Host "Branchez ou rebranchez l'ESP32 maintenant. Attente pendant $TimeoutSeconds secondes..."

  while ((Get-Date) -lt $deadline) {
    Start-Sleep -Milliseconds 1000

    $currentPorts = Get-SerialPorts -ArduinoCliPath $ArduinoCliPath
    $newPorts = @($currentPorts | Where-Object { $baseline -notcontains $_ })

    if ($newPorts.Count -eq 1) {
      return $newPorts[0]
    }

    if ($newPorts.Count -gt 1) {
      throw "Plusieurs nouveaux ports detectes: $($newPorts -join ', '). Relance avec -Port COMx."
    }
  }

  throw "Aucun nouveau port detecte dans les $TimeoutSeconds secondes. Relance avec -Port COMx si necessaire."
}

function Resolve-UploadPort {
  param(
    [string]$ExplicitPort,
    [string]$ArduinoCliPath,
    [switch]$UseWaitForDevice,
    [int]$TimeoutSeconds
  )

  if ($ExplicitPort) {
    return $ExplicitPort.ToUpperInvariant()
  }

  $ports = Get-SerialPorts -ArduinoCliPath $ArduinoCliPath

  if ($UseWaitForDevice) {
    return Wait-ForNewSerialPort -ExistingPorts $ports -ArduinoCliPath $ArduinoCliPath -TimeoutSeconds $TimeoutSeconds
  }

  if ($ports.Count -eq 1) {
    return $ports[0]
  }

  if ($ports.Count -eq 0) {
    return Wait-ForNewSerialPort -ExistingPorts @() -ArduinoCliPath $ArduinoCliPath -TimeoutSeconds $TimeoutSeconds
  }

  return Wait-ForNewSerialPort -ExistingPorts $ports -ArduinoCliPath $ArduinoCliPath -TimeoutSeconds $TimeoutSeconds
}

function Resolve-RecoveryFqbn {
  param(
    [object[]]$UploadOutput,
    [string]$CurrentFqbn
  )

  $outputText = ($UploadOutput | ForEach-Object { "$_" }) -join "`n"

  if ($outputText -notmatch 'This chip is ([A-Za-z0-9\-]+), not') {
    return $null
  }

  $chipName = $Matches[1].ToUpperInvariant()

  switch ($chipName) {
    'ESP32-S3' { return 'esp32:esp32:esp32s3' }
    'ESP32-S2' { return 'esp32:esp32:esp32s2' }
    'ESP32-C3' { return 'esp32:esp32:esp32c3' }
    'ESP32-C6' { return 'esp32:esp32:esp32c6' }
    'ESP32-H2' { return 'esp32:esp32:esp32h2' }
    'ESP32-P4' { return 'esp32:esp32:esp32p4' }
    'ESP32' { return 'esp32:esp32:esp32' }
    default { return $null }
  }
}

function Invoke-ArduinoCompile {
  param(
    [string]$ArduinoCliPath,
    [string]$TargetFqbn,
    [string]$TargetSketchPath
  )

  Write-Host "Compilation avec: $TargetFqbn"
  & $ArduinoCliPath compile --fqbn $TargetFqbn $TargetSketchPath

  if ($LASTEXITCODE -ne 0) {
    throw 'La compilation a echoue, upload annule.'
  }
}

function Invoke-ArduinoUpload {
  param(
    [string]$ArduinoCliPath,
    [string]$TargetPort,
    [string]$TargetFqbn,
    [string]$TargetSketchPath
  )

  $output = & $ArduinoCliPath upload -p $TargetPort --fqbn $TargetFqbn $TargetSketchPath 2>&1
  $exitCode = $LASTEXITCODE

  foreach ($line in $output) {
    Write-Host $line
  }

  return [PSCustomObject]@{
    ExitCode = $exitCode
    Output = @($output)
  }
}

$resolvedPort = Resolve-UploadPort -ExplicitPort $Port -ArduinoCliPath $CliPath -UseWaitForDevice:$WaitForDevice -TimeoutSeconds $WaitTimeoutSeconds
$effectiveFqbn = $Fqbn

Write-Host "Port selectionne: $resolvedPort"
Write-Host "FQBN: $effectiveFqbn"
Write-Host "Sketch: $SketchPath"

if (-not $SkipCompile) {
  Invoke-ArduinoCompile -ArduinoCliPath $CliPath -TargetFqbn $effectiveFqbn -TargetSketchPath $SketchPath
}

$uploadResult = Invoke-ArduinoUpload -ArduinoCliPath $CliPath -TargetPort $resolvedPort -TargetFqbn $effectiveFqbn -TargetSketchPath $SketchPath

if ($uploadResult.ExitCode -ne 0) {
  $recoveryFqbn = Resolve-RecoveryFqbn -UploadOutput $uploadResult.Output -CurrentFqbn $effectiveFqbn

  if ($recoveryFqbn -and $recoveryFqbn -ne $effectiveFqbn) {
    Write-Host "Chip detecte pendant l'upload. Nouvelle cible: $recoveryFqbn"
    $effectiveFqbn = $recoveryFqbn
    Invoke-ArduinoCompile -ArduinoCliPath $CliPath -TargetFqbn $effectiveFqbn -TargetSketchPath $SketchPath
    $uploadResult = Invoke-ArduinoUpload -ArduinoCliPath $CliPath -TargetPort $resolvedPort -TargetFqbn $effectiveFqbn -TargetSketchPath $SketchPath
  }

  if ($uploadResult.ExitCode -ne 0) {
    throw 'L''upload a echoue.'
  }
}

Write-Host 'Upload termine.'
