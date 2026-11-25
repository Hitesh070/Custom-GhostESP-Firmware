# Dependency Map — GhostESP Firmware & Desktop Command Center

## Managed ESP-IDF Components (`main/idf_component.yml` & Build Lock)

| Dependency | Resolved Version | Role / Consumer |
|---|---|---|
| `idf` | 6.1.1 | ESP-IDF core RTOS, Wi-Fi, NimBLE, USB, SPI, I2C, RMT, Temp Sensor |
| `espressif/cjson` | 1.7.19~2 | Config / telemetry JSON parsing |
| `espressif/cmake_utilities` | 0.5.3 | CMake component macros |
| `espressif/elf_loader` | 1.3.1 (local component) | Native SD apps ELF loader |
| `espressif/esp_io_expander` / `tca95xx_16bit` | 1.2.1 / 2.0.2 | Optional external I2C GPIO expanders |
| `espressif/esp_lcd_axs15231b` / `esp_lcd_touch` | 1.0.0 / 1.2.1 | Optional LCD/touch drivers |
| `espressif/esp_tinyusb` / `tinyusb` | 2.4.0 / 0.21.0~6 | Native USB-OTG BadUSB HID & MSC passthrough |
| `espressif/usb` / `usb_host_hid` | 1.6.1 / 1.2.1 | USB Host HID Keyboard mode |
| `espressif/w5500` | 1.0.1 | Optional Ethernet W5500 driver |

## Desktop Application Python Dependencies (`desktop/`)

| Package | Purpose | Status in Audit Env |
|---|---|---|
| `PySide6` | Industry-grade Qt6 GUI shell, threaded signals, 17-view stacked suite | Verified Installed |
| `matplotlib` | Real-time dark-themed thermal & NRF24 spectrum charts (`FigureCanvasQTAgg`) | Verified Installed |
| `pyserial` | Thread-safe COM port enumeration, buffered RX/TX, DTR/RTS controls | Verified Installed |
| `sqlite3` (stdlib) | WAL-mode persistent session, AP, BLE, deauth, temperature & timeline DB | Verified Built-in |
| `pytest` | Protocol parser, DB roundtrip, and offscreen PySide6 GUI regression tests | Verified Installed (7/7 passing) |
