# Third-party notices

The firmware is built with the Arduino core for ESP32 and the libraries below. They are not stored in this repository: `arduino-cli` downloads them at build time, and they keep their own licenses, whose full text ships with each of them.

| Component | Version used for the releases | License | Role |
| --- | --- | --- | --- |
| [Arduino core for ESP32](https://github.com/espressif/arduino-esp32) | 3.3.x | LGPL 2.1 | WiFi, TCP, web server, mDNS, OTA, preferences |
| [ESP-IDF](https://github.com/espressif/esp-idf) (inside the core) | 5.5 | Apache 2.0 | system, WiFi driver, lwIP |
| [ArduinoJson](https://github.com/bblanchon/ArduinoJson) | 7.4.x | MIT | JSON for the web API |
| [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) | 1.15.x | LGPL 3.0 | WS2812B LEDs and 8x8 matrix |

The release binaries link these components statically. Under the LGPL, you can rebuild the firmware against a modified version of the core or of Adafruit NeoPixel: the full source is in this repository and the build steps are in the [README](README.md#️-build-from-source).
