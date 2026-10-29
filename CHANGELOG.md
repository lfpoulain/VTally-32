# Changelog

Firmware versions, newest first. The version is `FIRMWARE_VERSION` in `vmix_tally_esp32/vmix_tally_esp32.ino`, shown in the **Diagnostics** tab and in `/diagnostics`. Each version bump pushed to `main` publishes a [GitHub release](https://github.com/lfpoulain/VTally-32/releases) with the binaries for both chips.

## 2.1.2: a tally you can trust (2026-10-29)

**Fixed**

- The tally asks vMix for the full tally state every second and drops the connection after 4 seconds of silence. A dead connection used to leave the light on a wrong state for about 35 seconds.
- A WiFi glitch no longer tears everything down. The WiFi driver reconnects on its own, the tally retries every 10 seconds, and the access point only reopens after 30 seconds without WiFi. It used to switch the radio off, erase the WiFi settings and restart the access point on every drop.
- WiFi power saving stays off after a reconnection (`WiFi.setSleep(false)`); it used to come back on each time the station restarted.
- Connecting to vMix gives up after 1.5 seconds instead of freezing the loop, waits a little longer after each failure (up to 5 seconds), and no longer loses the bytes that arrive right after `SUBSCRIBE OK`.
- A connection state that could leave the tally deaf to vMix until a reboot.

**Changed**

- The serial log defaults to `Normal` instead of `Debug`.

## 2.1.1: tally resync (2026-04-29)

**Fixed**

- After subscribing, the tally asks vMix for the current state (`TALLY`) instead of waiting for the next cut.
- A subscription refused by vMix (`SUBSCRIBE ER`) is detected and retried.
- Room for 1024 characters per vMix line, for productions with many inputs.

## 2.1.0: lower latency (2026-04-09)

**Added**

- Serial log level (`None`, `Errors`, `Normal`, `Debug`) in **Matériel & Réseau**, stored in flash.
- TCP keepalive on the vMix connection, to notice a vanished vMix in about 35 seconds rather than minutes.

**Changed**

- WiFi power saving turned off, tally data handled first in the loop, loop delay down from 10 ms to 1 ms, WiFi status cached.

## 2.0.2: 8x8 matrix and live debug (2026-04-03)

**Added**

- Display modes: `Tally simple` or `Écran 8x8` (64 LEDs).
- Live debug on the 8x8 matrix: a digit for each step of the startup, from joining WiFi to vMix connected.
- `POST /vmix/reconnect` and a **Forcer une reconnexion VMix** button.

**Changed**

- vMix lines read into a fixed buffer, and the connection state tracked step by step.

## 2.0.0: automatic releases (2026-03-26)

**Added**

- Automatic builds for ESP32 and ESP32-S3, published as a GitHub release when `FIRMWARE_VERSION` changes, with the firmware version shown in **Diagnostics**.
- Serial log on the native USB port of the ESP32-S3 (USB CDC on boot).

## 1.0.0 to 1.0.2 (2026-03-26)

First tagged builds, while the release pipeline was being set up.

**Added**

- Firmware update from the web interface (`POST /update`), in 1.0.1.
- Tally names turned into valid hostnames for WiFi and mDNS, in 1.0.0.
