# VTally-32

<p align="center">
  <strong>WiFi tally light for vMix based on ESP32 / ESP32-S3</strong><br>
  Modern web UI, persistent configuration, NeoPixel support, built-in diagnostics, and an <code>arduino-cli</code> workflow.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/platform-ESP32%20%7C%20ESP32--S3-00979D?style=for-the-badge" alt="Platform">
  <img src="https://img.shields.io/badge/framework-Arduino-00979D?style=for-the-badge" alt="Framework">
  <img src="https://img.shields.io/badge/interface-Web_UI-6366F1?style=for-the-badge" alt="Web UI">
  <img src="https://img.shields.io/badge/license-NC%20Attribution-orange?style=for-the-badge" alt="License">
</p>

---

## Overview

**VTally-32** is an Arduino firmware project for ESP32 boards that turns an ESP32 into a **WiFi tally light for vMix**.

The project includes:

- a **vMix TCP connection** with exponential backoff and automatic reconnection
- an embedded **web interface** to configure the device without recompiling
- **mDNS support** (`http://[tally-name].local`)
- configurable **NeoPixel / WS2812B** support
- persistent configuration via `Preferences`
- an **AP + STA** workflow for first-time setup and recovery
- a built-in **diagnostics page** in the web UI
- a simple **build / upload workflow with `arduino-cli`**

---

## Table of Contents

- [Features](#features)
  - [Performance & Low Latency](#performance--low-latency)
- [Project Architecture](#project-architecture)
- [Hardware](#hardware)
- [Wiring](#wiring)
- [Quick Start](#quick-start)
- [Web Configuration](#web-configuration)
- [Build and Upload](#build-and-upload)
- [Web Interface](#web-interface)
- [REST API](#rest-api)
- [Serial Logs](#serial-logs)
- [Repository Structure](#repository-structure)
- [License](#license)

---

## Features

### vMix Connection

- official vMix TCP protocol on fixed port `8099` (vMix API)
- `TALLY` subscription handling
- smart automatic reconnection with exponential backoff (up to 30s)
- memory-leak-proof TCP socket handling
- proper handling of `LIVE`, `PREVIEW`, and `LIVE+PREVIEW`
- TCP keepalive for fast detection of dead connections (~35s instead of 2min+)

### LED Management

- `Adafruit NeoPixel` support
- compatible with WS2812 / WS2812B
- configurable `Live`, `Preview`, and `Off` colors
- brightness control from `0` to `255`
- configurable LED count
- highly optimized loop: renders only when states change

### Network & WiFi Management

- `AP + STA` mode
- custom device naming (`tally_name`) mapped to AP SSID and hostname
- mDNS resolution (reach your device at `http://vtally-32.local`)
- non-blocking asynchronous WiFi scanning from the UI (limits to 15 networks)
- silent periodic reconnection with immediate disconnect detection
- automatic AP fallback if WiFi drops
- WiFi power save mode disabled for lowest possible latency

### Performance & Low Latency

- WiFi power save mode (`modem sleep`) disabled to prevent 100-300ms radio wake-up delays
- tally data processing has highest priority in the main loop (runs before web server and WiFi checks)
- fixed-size `char[]` buffer for vMix TCP parsing (no heap fragmentation from `String` concatenation)
- `WiFi.status()` result cached to avoid repeated driver calls
- rate-limited WiFi monitoring (500ms) to free CPU for tally processing
- chunked HTML serving with `yield()` to prevent blocking during web page loads
- configurable serial log level (None / Error / Normal / Debug) to eliminate up to ~13ms of `Serial.printf` overhead per tally change in production
- loop delay reduced from 10ms to 1ms

### Reliability and Maintenance

- PROGMEM-based static web page serving (no RAM fragmentation)
- asynchronous, clean reboots (avoids frontend network errors)
- persistent configuration with validation
- configurable serial log level with 4 verbosity modes
- modular firmware split across multiple `.ino` files
- integrated diagnostics page
- TCP keepalive detects vMix server crashes in ~35s

---

## Project Architecture

The firmware was modularized to make the project easier to maintain.

| File | Role |
|---|---|
| `vmix_tally_esp32.ino` | constants, globals, `setup()`, `loop()` |
| `config.ino` | configuration validation, load, and save logic |
| `wifi.ino` | WiFi connection, mDNS, access point, reconnection |
| `vmix.ino` | TCP socket, vMix subscription, tally parsing, backoff |
| `led.ino` | LED state management |
| `web.ino` | web UI and REST handlers |
| `diagnostics.ino` | `/diagnostics` endpoint and system metrics |

---

## Hardware

### Supported Boards

- ESP32
- ESP32-S3

### Required Parts

- 1x ESP32 or ESP32-S3 board
- 1x NeoPixel / WS2812B LED or compatible strip/bar
- jumper wires
- suitable external power if using several LEDs

### Default Firmware Values

- Tally Name: `VTally-32`
- LED GPIO: `14`
- LED count: `1`
- `Live` color: `#FF0000`
- `Preview` color: `#00FF00`
- `Off` color: `#000000`
- brightness: `255`
- AP Password: `vtally32`

---

## Wiring

### Simple Example

```text
ESP32 / ESP32-S3      WS2812B
----------------      -------
GPIO 14        ---->  DIN
3.3V / 5V      ---->  VCC
GND            ---->  GND
```

> For more than 10 LEDs, an external 5V power supply is strongly recommended.

---

## Quick Start

### 1. Flash the firmware

You can use Arduino IDE, but the repository is now ready for `arduino-cli`.

### 2. Connect to the access point

On first boot, the device creates an access point using its default name:

- **SSID**: `VTally-32` (or your custom name)
- **Password**: `vtally32`

Then open:

```text
http://192.168.4.1
```

### 3. Configure WiFi

In the **WiFi** tab:

- run a scan
- select your network
- enter the password
- save

The ESP restarts and joins your local network.

### 4. Configure vMix

In the **VMix & Colors** tab:

- enter the vMix PC IP address
- choose the input to monitor
- adjust colors and brightness

*Note: The TCP connection always uses port `8099`.*

---

## Web Configuration

The web interface lets you configure the device without editing the code. You can access it via IP or via mDNS (e.g. `http://vtally-32.local`).

### `VMix & Colors` Tab

| Parameter | Description |
|---|---|
| `VMix IP` | IP address of the vMix machine |
| `Input Number` | input number to monitor |
| `Live` | color shown when the input is live |
| `Preview` | color shown when the input is in preview |
| `Off` | color shown when the input is off |
| `Brightness` | LED intensity from `0` to `255` |

### `Hardware & Network` Tab

| Parameter | Description |
|---|---|
| `Tally Name` | Custom name used for mDNS, Hostname, and AP SSID |
| `LED GPIO Pin` | NeoPixel output pin |
| `LED Count` | number of LEDs in the chain |
| `Display Mode` | `Tally simple` or `Ecran 8x8` (64 LED matrix) |
| `Live Debug` | show numeric stage code on the 8x8 matrix (8x8 mode only) |
| `Serial Log Level` | `None` (production), `Errors only`, `Normal`, `Debug (verbose)` |

> Hardware/Network changes trigger an automatic restart.
>
> **Tip:** Set `Serial Log Level` to `None` during live productions to eliminate serial output latency.

### `WiFi` Tab

- scan available networks
- select or manually enter the SSID
- save the WiFi password
- automatic restart after applying the settings

### `Diagnostics` Tab

Shows live information such as:

- tally name
- uptime
- free heap / minimum heap / largest alloc block
- WiFi mode
- AP / STA state
- SSID, STA IP, AP IP, RSSI
- vMix connection and socket state
- vMix target and monitored input
- current tally state
- active LED configuration

---

## Build and Upload

### `arduino-cli` dependencies

```powershell
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' config init --overwrite
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' core update-index
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' core install esp32:esp32
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' lib install "ArduinoJson"
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' lib install "Adafruit NeoPixel"
```

### Compile manually

#### ESP32

```powershell
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' compile --fqbn esp32:esp32:esp32 ".\vmix_tally_esp32"
```

#### ESP32-S3

```powershell
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' compile --fqbn esp32:esp32:esp32s3 ".\vmix_tally_esp32"
```

For `ESP32-S3`, the repository build flow enables `USB CDC on boot` automatically in CI and in `upload-esp32.ps1`, so the serial monitor remains available after flashing or OTA updates when using the native USB port.


### OTA update from the web UI

The firmware now supports direct OTA updates from the built-in web interface.

Requirements:

- the device must already be running a firmware version that includes OTA support
- the device must be reachable on the network or via its AP
- the selected file must match the target board (`ESP32` or `ESP32-S3`)
- use the dedicated application image ending with `-ota.bin`

Procedure:

- open the web interface
- go to the `WiFi` tab
- select the firmware file ending with `-ota.bin`
- start the OTA update
- wait for the upload to complete and for the ESP32 to reboot automatically

Important:

- do **not** use `bootloader` or `partitions` binaries in the OTA form
- OTA is intended for firmware application updates only
- if the uploaded binary targets the wrong board, the update can fail and will be rejected by the updater
- on `ESP32-S3`, the OTA binary should be built with `USB CDC on boot` enabled if you want logs on the native USB serial monitor after reboot

### Display modes and live debug

The hardware settings page now supports two display modes:

- `Tally simple`: classic single LED or LED strip tally behavior
- `Écran 8x8`: forces the device to use `64` LEDs for an `8x8` matrix

When `Live debug` is enabled on `Écran 8x8`, the matrix displays a numeric stage code for fast troubleshooting during setup and live operation:

- `1` = WiFi connection in progress
- `2` = vMix connection / reconnection in progress
- `3` = vMix connected and subscribed
- `4` = tally currently in preview
- `5` = tally currently live
- `6` = vMix connection error or disconnection
- `7` = AP only mode
- `8` = WiFi ready
- `9` = WiFi failure

### Automatic upload script

The repository includes this PowerShell script:

```text
upload-esp32.ps1
```

Script features:

- automatic COM port detection when the board is plugged in
- manual override with `-Port COMx`
- compile + upload in a single command
- automatic board fallback if the detected chip is different, for example `ESP32-S3`

#### Recommended usage

```powershell
powershell -ExecutionPolicy Bypass -File .\upload-esp32.ps1
```

#### Force a port

```powershell
powershell -ExecutionPolicy Bypass -File .\upload-esp32.ps1 -Port COM3
```

#### Force a board target

```powershell
powershell -ExecutionPolicy Bypass -File .\upload-esp32.ps1 -Fqbn esp32:esp32:esp32s3
```

#### Increase plug-in wait time

```powershell
powershell -ExecutionPolicy Bypass -File .\upload-esp32.ps1 -WaitTimeoutSeconds 60
```

---

## Web Interface

### Dynamic Header

The header changes visually depending on the current state:

| State | Badge |
|---|---|
| disconnected | `Disconnected` |
| connected | `Connected` |
| live | `LIVE` |
| preview | `PREVIEW` |

### UI Highlights

- modern responsive layout
- real-time configuration updates
- integrated WiFi scanning
- diagnostics page without external tools
- reboot action from the interface

---

## REST API

### Available Endpoints

| Endpoint | Method | Description |
|---|---|---|
| `/` | `GET` | full web interface |
| `/config` | `GET` | read configuration |
| `/config` | `POST` | update configuration |
| `/status` | `GET` | real-time tally state |
| `/diagnostics` | `GET` | detailed system metrics |
| `/scan` | `GET` | scan WiFi networks |
| `/wifi` | `POST` | save WiFi configuration |
| `/vmix/reconnect` | `POST` | force a vMix disconnect/reconnect cycle |
| `/update` | `POST` | upload and install OTA firmware |
| `/reboot` | `POST` | reboot the device |

### Example `GET /status`

```json
{
  "connected": true,
  "live": false,
  "preview": true,
  "vmix_host": "192.168.1.100",
  "vmix_input": "3",
  "wifi_ssid": "StudioWiFi",
  "wifi_ip": "192.168.1.50"
}
```

### Example `GET /diagnostics`

```json
{
  "firmware_version": "2.1.0",
  "tally_name": "Cam-1",
  "uptime_human": "0d 00h 12m 05s",
  "free_heap": 231456,
  "wifi_mode": "STA",
  "ap_active": false,
  "sta_connected": true,
  "wifi_ssid": "StudioWiFi",
  "wifi_ip": "192.168.1.50",
  "ap_ip": "192.168.4.1",
  "wifi_rssi": -49,
  "vmix_connected": true,
  "vmix_socket_connected": true,
  "vmix_host": "192.168.1.100",
  "vmix_port": 8099,
  "vmix_input": "3",
  "tally_state": "PREVIEW",
  "led_pin": 14,
  "led_count": 1,
  "brightness": 255,
  "log_level": 3
}
```

---

## Serial Logs

Serial logging is configurable via the web UI (`Hardware & Network` tab) with four levels:

| Level | Value | Output | Use case |
|---|---|---|---|
| **None** | `0` | No serial output | Live production (lowest latency) |
| **Errors** | `1` | `[ERROR]` and `[WARN]` only | Production with safety net |
| **Normal** | `2` | + `[INFO]`, `[NET]`, `[VMIX]`, `[WEB]` | General use |
| **Debug** | `3` | + `[DEBUG]` verbose messages | Troubleshooting |

The setting is persisted in flash and survives reboots.

At 115200 baud, each log line takes ~4ms of blocking serial output. During a live production with frequent tally changes, setting the level to `None` eliminates ~13ms of latency per state change.

---

## Repository Structure

```text
.
├── LICENSE
├── README.md
├── upload-esp32.ps1
└── vmix_tally_esp32
    ├── vmix_tally_esp32.ino
    ├── config.ino
    ├── wifi.ino
    ├── vmix.ino
    ├── led.ino
    ├── web.ino
    └── diagnostics.ino
```

---

## License

This project is distributed under the **VTally-32 Non-Commercial Attribution License 1.0**.

You may:

- use the project for personal, educational, research, and other non-commercial purposes
- modify the code
- redistribute original or modified versions

You must:

- always credit the original author: `LFPoulain`
- keep the license text with redistributed copies
- clearly mark modified versions as modified

You may not:

- use the project commercially without prior written permission

See [LICENSE](LICENSE) for the full text.

---

<p align="center">
  Developed by <strong>LFPoulain</strong> for the vMix community.<br>
  If this project helps you, consider giving it a star.
</p>
