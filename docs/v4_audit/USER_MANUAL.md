# GhostESP V4 User Manual & Command Quickstart

## 1. Connecting to the Hardware (`7Semi ESP32-S3-WROOM-1-N16R8`)
- **`COM5` (Native USB-Serial/JTAG, 115200 baud):** Primary GhostESP CLI shell. Automatically disconnects if BadUSB / USB Host HID takes over `GPIO19/20`.
- **`COM6` (CP210x Hardware UART0, 115200 baud):** Independent hardware UART console on `GPIO43/44`; stays connected even during TinyUSB BadUSB / USB Host tests.
- **Desktop GUI (`python desktop/app.py`):** Launches **GhostESP Command Center V4** (PySide6 + Matplotlib + SQLite WAL).

## 2. Core Research Workflows
- **Temperature Telemetry:** `temp`, `temp json`, `temp f`, `temp c`, `temp interval <500..60000>`, `temp history`, `temp reset`
- **Hardware Self-Test:** `selftest` (reports SRAM, 1 MB Octal PSRAM alloc, internal die temp, I2C0 scan, SPI2 NRF24 check, SD card, UART1, USB-OTG, RGB NeoPixel)
- **Wi-Fi Discovery & Defense:** `scanap`, `stopscan` (preserves AP cache & stops BLE if active), `list -a`, `wpa3check`, `congestion`, `roguedetect start|status|stop|clear`, `deauthmon start|status|stop|clear`
- **BLE Discovery:** `bledetect`, `blescan [-adv|-f|-ds|-a|-r|-g|-s]`, `listairtags`, `listflippers`, `stopscan`
- **NRF24 RF Spectrum (Optional SPI2 Module):** `nrf24lab diag`, `nrf24lab sweep <samples>`, `nrf24lab benchmark <cycles>`
- **BadUSB HID Lab (Authorized Testing Only):** `badusb list`, `badusb run <script|builtin> --confirm`, `badusb exec <size> --confirm`, `badusb stop`, `usbkbd <on|off|status>`
- **Profiles & Diagnostics:** `profile list`, `profile apply <0..7>`, `help godmode`, `mem`, `ps`, `df`, `chipinfo`
