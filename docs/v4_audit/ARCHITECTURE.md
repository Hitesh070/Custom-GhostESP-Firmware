# System Architecture (GhostESP God Mode V4)

## 1. Flash Partition Layout ([`partitions.csv`](../../partitions.csv))

| Partition | Type | SubType | Offset | Size | Purpose |
|---|---|---|---|---|---|
| `nvs` | `data` | `nvs` | `0x9000` | `0x7000` (28 KB) | Non-volatile settings & country codes |
| `app0` | `app` | `factory` | `0x10000` | `0x3E0000` (3,968 KB) | GhostESP V4 firmware image (`0x38e610` bytes used, ~8% headroom) |
| `coredump` | `data` | `coredump` | `0x3F0000` | `0x10000` (64 KB) | ELF coredump snapshot partition |

## 2. Firmware Layering (`main/` & `include/`)
1. **Hardware & Board Registry (`main/hardware/board_config.c`):** Single source of truth locking reserved silicon GPIOs (Flash `27–32`, Octal PSRAM `33–38`, Native USB `19–20`), tracking peripheral presence (`HW_PRESENCE_*`), and scanning `I2C0` / `SPI2`.
2. **Managers (`main/managers/`):** Coexistence & lifecycle owners for Wi-Fi (`wifi_manager.c`), AP/HTTP (`ap_manager.c`), BLE NimBLE (`ble_manager.c`), USB-OTG (`badusb_manager.c`, `usb_keyboard_manager.c`, `usb_msc_manager.c`), SD/FATFS (`sd_card_manager.c`), Display (`display_manager.c`), Infrared (`infrared_manager.c`), Settings (`settings_manager.c`).
3. **Security Research & Telemetry Modules (`main/modules/`):** V4 thermal monitor (`temp_monitor.c`), passive Evil Twin detector (`rogue_detector.c`), passive deauth monitor (`deauth_monitor.c`), 126-ch NRF24 RF analyzer (`nrf24_lab.c`), automated bus self-test (`self_test.c`), profile presets (`profile_manager.c`), developer radar/achievements (`ghost_eastereggs.c`).
4. **CLI Command Dispatch (`main/core/`):** 223 commands registered in `commandline.c`, formatted via `glog.c` with explicit `fflush(stdout)`.

## 3. Desktop Suite (`desktop/`)
- **UI Presentation (`app.py`, `theme.py`):** 17 engineering views + dark Matplotlib charts.
- **Transport (`serial_comm.py`):** Multi-threaded queued transceiver + host SDR tool probe.
- **Telemetry Ingestion (`parsers.py`):** Zero-crash incremental parser converting raw serial lines into typed `ParsedEvent` records.
- **Persistence (`database.py`):** 7-table SQLite WAL store.
