# VTally-32

<p align="center">
  <strong>WiFi tally light for vMix based on ESP32 / ESP32-S3</strong><br>
  Modern web UI, persistent configuration, NeoPixel support, built-in diagnostics, and an <code>arduino-cli</code> workflow.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/platform-ESP32%20%7C%20ESP32--S3-00979D?style=for-the-badge" alt="Platform">
  <img src="https://img.shields.io/badge/framework-Arduino-00979D?style=for-the-badge" alt="Framework">
  <img src="https://img.shields.io/badge/interface-Web_UI-6366F1?style=for-the-badge" alt="Web UI">
  <img src="https://img.shields.io/badge/license-CC%20BY--NC%204.0-orange?style=for-the-badge" alt="License CC BY-NC 4.0">
</p>

---

## Overview

**VTally-32** is an Arduino firmware for ESP32 that turns an ESP32 board into a **WiFi tally light for vMix**.

The project provides:

- a **vMix TCP TALLY** connection with automatic reconnection
- **HTTP/XML retrieval** of vMix inputs for the web interface
- a built-in **web UI** to configure the device without recompiling
- configurable **NeoPixel / WS2812B** support
- persistent configuration through `Preferences`
- an **AP + STA** mode for first-time setup and network recovery
- a built-in **system diagnostics** page
- a **build / upload workflow with `arduino-cli`**

---

## Table of Contents

- [Features](#features)
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
- [Roadmap](#roadmap)
- [License](#license)

---

## Features

### vMix Connection

- **TCP TALLY** connection on fixed port `8099`
- `SUBSCRIBE TALLY` support
- **HTTP/XML API** requests on a configurable Web Controller port
- automatic reconnection every 5 seconds
- correct handling of `LIVE`, `PREVIEW`, and `LIVE+PREVIEW`
- source selection by **number** or by **vMix `key`**
- periodic `key -> current number` remapping so a source stays tracked even when its position changes

### LED Control

- `Adafruit NeoPixel` support
- compatible with WS2812 / WS2812B
- configurable colors for `Live`, `Preview`, and `Off`
- brightness adjustment from `0` to `255`
- configurable LED count
- centralized visual state updates

### WiFi Networking

- `AP + STA` mode
- local access point for initial setup
- WiFi scanning from the UI
- silent periodic reconnection
- automatic AP re-enable if WiFi is lost

### Reliability and Maintenance

- persistent configuration with strict validation
- structured serial logs
- modular firmware split across `.ino` files
- integrated diagnostics page
- automation-friendly `arduino-cli` build workflow

---

## Project Architecture

The firmware has been modularized to make maintenance easier.

| File | Role |
|---|---|
| `vmix_tally_esp32.ino` | constants, globals, `setup()`, `loop()` |
| `config.ino` | validation, loading, and saving configuration |
| `wifi.ino` | WiFi connection, AP mode, reconnection |
| `vmix.ino` | TCP socket, vMix subscription, tally parsing |
| `led.ino` | LED state handling |
| `web.ino` | web UI and REST endpoints |
| `diagnostics.ino` | `/diagnostics` endpoint and system metrics |

---

## Hardware

### Supported Boards

- ESP32
- ESP32-S3

### Required Components

- 1x ESP32 or ESP32-S3 board
- 1x NeoPixel / WS2812B LED or compatible bar
- jumper wires
- suitable power supply if using multiple LEDs

### Default Firmware Values

- LED GPIO: `14`
- LED count: `1`
- `Live` color: `#FF0000`
- `Preview` color: `#00FF00`
- `Off` color: `#000000`
- brightness: `255`
- AP SSID: `VTally-32`
- AP password: `vtally32`

---

## Wiring

### Basic Example

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

### 1. Flash the Firmware

You can use the Arduino IDE, but the repository is ready for `arduino-cli`.

### 2. Connect to the Access Point

On first boot, the device creates an AP:

- **SSID**: `VTally-32`
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

The ESP will restart and join your local network.

### 4. Configure vMix

In the **VMix & Colors** tab:

- enter the vMix PC IP address
- set the **HTTP / XML API port** used by the vMix Web Controller, usually `8088`
- note that the **TCP TALLY port** is fixed to `8099`
- choose **Fixed input** mode for the lightest and simplest behavior
- or enable **Tracked source by key** mode to follow a source even if its number changes
- configure either the **fixed input** or the **tracked source** depending on the selected mode
- adjust colors and brightness

---

## Web Configuration

The web interface lets you control everything without changing the code.

### `VMix & Colors` Tab

| Setting | Description |
|---|---|
| `vMix IP` | IP address of the vMix machine |
| `vMix HTTP / XML API Port (Web Controller)` | Web Controller port used for `GET /API/?`, usually `8088` |
| `TCP TALLY Port` | fixed TCP port used for `SUBSCRIBE TALLY`, always `8099` |
| `Tracking mode` | `Fixed input (fast)` or `Tracked source by key` |
| `Tracked source` | dropdown of vMix inputs fetched from the XML API, shown only in `key` mode |
| `Fixed input` | manual input number to monitor, shown only in fixed mode |
| `Key tracking refresh (seconds)` | remap interval for `key -> current number` when key tracking is enabled |
| `Tracked source name` | title currently stored for the tracked `key` |
| `Tracked key` | stable vMix identifier used to follow a source when its order changes |
| `Live` | color shown when live |
| `Preview` | color shown when in preview |
| `Off` | color shown when off |
| `Brightness` | LED intensity from `0` to `255` |

### vMix Tracking Modes

- **`Fixed input (fast)`**
  - reads the `vmix_input` position directly from the `TALLY` stream
  - minimal overhead
  - ideal if the input order in vMix does not change

- **`Tracked source by key`**
  - the source is selected in the **Tracked source** dropdown
  - the firmware stores the vMix `key`
  - periodic XML API refresh resolves the `key` to the **current input number**
  - as soon as a new number is detected for that `key`, the last tally state is **reapplied automatically**
  - the tally stays attached to the correct source even if it moves in vMix

### `Hardware` Tab

| Setting | Description |
|---|---|
| `LED GPIO Pin` | NeoPixel output pin |
| `LED Count` | number of LEDs in the chain |

> Any hardware change triggers an automatic restart.

### `WiFi` Tab

- scan available networks
- select a network or enter the SSID manually
- save the password
- automatic restart

### `Diagnostics` Tab

Displays live:

- uptime
- free / minimum / maximum allocatable heap
- WiFi mode
- AP / STA state
- SSID, STA IP, AP IP, RSSI
- vMix state and socket state
- **HTTP API** target and **TCP TALLY** target
- monitored input, tracking mode, `key`, and resolved input
- current tally state
- active LED configuration

---

## Build and Upload

### `arduino-cli` Dependencies

```powershell
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' config init --overwrite
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' core update-index
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' core install esp32:esp32
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' lib install "ArduinoJson"
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' lib install "Adafruit NeoPixel"
```

### Manual Compile

#### ESP32

```powershell
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' compile --fqbn esp32:esp32:esp32 ".\vmix_tally_esp32"
```

#### ESP32-S3

```powershell
& 'C:\Program Files\Arduino CLI\arduino-cli.exe' compile --fqbn esp32:esp32:esp32s3 ".\vmix_tally_esp32"
```

### Automatic Upload Script

The repository includes a PowerShell script:

```text
upload-esp32.ps1
```

Script features:

- automatic detection of a newly connected COM port
- manual override with `-Port COMx`
- compile + upload in one command
- automatic target correction if the detected chip differs, for example `ESP32-S3`

#### Recommended Usage

```powershell
powershell -ExecutionPolicy Bypass -File .\upload-esp32.ps1
```

#### Force a Port

```powershell
powershell -ExecutionPolicy Bypass -File .\upload-esp32.ps1 -Port COM3
```

#### Force the Board Target

```powershell
powershell -ExecutionPolicy Bypass -File .\upload-esp32.ps1 -Fqbn esp32:esp32:esp32s3
```

#### Increase the Device Detection Timeout

```powershell
powershell -ExecutionPolicy Bypass -File .\upload-esp32.ps1 -WaitTimeoutSeconds 60
```

---

## Web Interface

### Dynamic Header

The header changes visually depending on the current state:

| State | Badge |
|---|---|
| disconnected | `⚠ Disconnected` |
| connected | `✓ Connected` |
| live | `🔴 LIVE` |
| preview | `🟢 PREVIEW` |

### UI Highlights

- modern and responsive design
- real-time configuration
- built-in WiFi scan
- vMix source selection by name with a dropdown
- stable source tracking by `key`
- diagnostics page without external tools
- restart from the interface

---

## REST API

### Available Endpoints

| Endpoint | Method | Description |
|---|---|---|
| `/` | `GET` | full web interface |
| `/config` | `GET` | read current configuration |
| `/config` | `POST` | update configuration |
| `/vmix/inputs` | `GET` | fetch the vMix input list through the XML API |
| `/status` | `GET` | real-time tally state |
| `/diagnostics` | `GET` | detailed system metrics |
| `/scan` | `GET` | scan WiFi networks |
| `/wifi` | `POST` | save WiFi configuration |
| `/reboot` | `POST` | restart the device |

### Example `GET /status`

```json
{
  "connected": true,
  "live": false,
  "preview": true,
  "vmix_host": "192.168.1.100",
  "vmix_port": 8088,
  "vmix_input": "3",
  "wifi_ssid": "StudioWiFi",
  "wifi_ip": "192.168.1.50"
}
```

`vmix_port` refers to the **HTTP/XML API port**. The TCP tally stream always uses `8099`.

### Example `GET /vmix/inputs`

```json
{
  "selected_input": "3",
  "selected_key": "55cbe357-a801-4d54-8ff2-08ee68766fae",
  "selected_title": "LateNightNews",
  "track_by_key": true,
  "inputs": [
    {
      "number": "1",
      "title": "Camera 1",
      "key": "26cae087-b7b6-4d45-98e4-de03ab4feb6b",
      "type": "Capture"
    },
    {
      "number": "3",
      "title": "LateNightNews",
      "key": "55cbe357-a801-4d54-8ff2-08ee68766fae",
      "type": "VirtualSet"
    }
  ]
}
```

### Example `GET /diagnostics`

```json
{
  "firmware_version": "2.0.0",
  "uptime_human": "0d 00h 12m 05s",
  "free_heap": 231456,
  "wifi_mode": "AP+STA",
  "ap_active": false,
  "sta_connected": true,
  "wifi_ssid": "StudioWiFi",
  "wifi_ip": "192.168.1.50",
  "ap_ip": "192.168.4.1",
  "wifi_rssi": -49,
  "vmix_connected": true,
  "vmix_socket_connected": true,
  "vmix_host": "192.168.1.100",
  "vmix_port": 8088,
  "vmix_api_port": 8088,
  "vmix_tcp_port": 8099,
  "vmix_input": "3",
  "vmix_track_by_key": true,
  "vmix_input_key": "55cbe357-a801-4d54-8ff2-08ee68766fae",
  "vmix_input_title": "LateNightNews",
  "vmix_key_refresh_seconds": 10,
  "resolved_vmix_input": 3,
  "resolved_vmix_key": "55cbe357-a801-4d54-8ff2-08ee68766fae",
  "resolved_vmix_title": "LateNightNews",
  "tally_state": "PREVIEW",
  "led_pin": 14,
  "led_count": 1,
  "brightness": 255
}
```

---

## Serial Logs

Recommended baud rate:

```text
115200
```

Example:

```text
[INFO] Configuration loaded:
[DEBUG]   WiFi: StudioWiFi
[DEBUG]   VMix API: 192.168.1.100:8088 TCP TALLY:8099 Input:3 TrackByKey:YES Refresh:10s
[NET] AP started: http://192.168.4.1
[NET] WiFi connected: StudioWiFi (192.168.1.50)
[VMIX] Connecting TCP TALLY to vMix 192.168.1.100:8099...
[VMIX] Tracked source resolved: 55cbe357-a801-4d54-8ff2-08ee68766fae -> input 3 (LateNightNews)
[VMIX] vMix connected and subscribed successfully
[INFO] TALLY: OFF → PREVIEW [Input 3]
```

---

## Repository Structure

```text
.
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

## Roadmap

- continued web UI improvements
- simpler diagnostics export
- broader hardware support across ESP32 boards
- even more automated flashing and debugging tools

---

## License

This project is distributed under the **CC BY-NC 4.0** license.

This allows:

- use
- modification
- redistribution

Under the following conditions:

- mandatory attribution to **LFPoulain**
- preservation of the author credit
- indication of any modifications made
- **no commercial use**

See [LICENSE](LICENSE) for the full text.

---

<p align="center">
  Developed by <strong>LFPoulain</strong> for the vMix community.<br>
  If this project helps you, a ⭐ on GitHub is always appreciated.
</p>
