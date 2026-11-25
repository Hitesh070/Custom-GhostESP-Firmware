# Feature Validation Matrix (GhostESP God Mode V4)

| Subsystem / Feature | Implementation File(s) | Live Bench / Automated Verification | Status | Notes |
|---|---|---|---|---|
| **ESP32-S3 Silicon Temperature Monitor** | `temp_monitor.c`, `cmd_godmode.c` | Verified via `temp`, `temp json`, `temp f`, `temp c`, `temp interval`, `temp history`, `temp status` (`51.2–54.2 C`, `0` errors) | PASS | Hardware `-10..80 C` curve active |
| **Wi-Fi AP Scan & Discovery** | `ap_scan.c`, `wifi_manager.c`, `cmd_wifi.c` | `scanap`, `stopscan`, `list -a`, `wpa3check`, `congestion` verified live on Iteration 2 COM5 (`6`–`13` APs preserved after `stopscan`) | PASS | Fixed AP cache wipe on `stopscan` |
| **Passive Rogue AP & Evil Twin Radar** | `rogue_detector.c` | `roguedetect start/status/stop` verified live (`7` tracked APs, `VITC-HOS2-4` multi-BSSID Enterprise classified `CLEAN`) | PASS | Eliminated Enterprise false positives |
| **Passive Deauth / Disassoc Monitor** | `deauth_monitor.c` | `deauthmon start/status/stop` verified live; `2 s` rate auto-decay & unlocked `glog()` fixed | PASS | Zero storms seen in clean lab |
| **BLE Stack & Scanners (`bledetect` / `blescan`)** | `ble_manager.c`, `cmd_ble.c` | Verified live (`bledetect`, `blescan -t`, `stopscan` stop); `6144`-byte `nimble_host` stack eliminates panic | PASS | `stopscan` BLE stop verified Iteration 2 |
| **NRF24L01+ Register & RF Spectrum Lab** | `nrf24_lab.c`, `cmd_nrf24.c` | Probed live over SPI2 (`CONFIG=0x00 / STATUS=0x00`); honest `HARDWARE UNRESPONSIVE` gating verified | HW DEPENDENT | Physical module disconnected |
| **Automated Hardware Self-Test** | `self_test.c`, `board_config.c` | Verified SRAM, `1 MB` PSRAM alloc, temp sensor, I2C0 scan, SPI2 NRF24 check, UART1, USB-OTG, RGB | PASS | External buses accurately flagged `HW REQUIRED` |
| **Research Profiles & Developer Console** | `profile_manager.c`, `ghost_eastereggs.c` | Verified `profile list/apply`, `ghost status/radar/achievements/easteregg/version` (`ELF-SHA256` reported) | PASS | `radar` shows real live counts & die temp |
| **BadUSB / USB Host Keyboard** | `badusb_manager.c`, `usb_keyboard_manager.c`, `cmd_badusb.c` | `--confirm` block verified live on `badusb run builtin` and `badusb exec 128`; mutual exclusion & `REPEAT` fixed in code | PASS (Safety) / HW MANUAL | HID typing detaches `COM5` USB CDC |
| **SD Card & FATFS Storage** | `sd_card_manager.c`, `cmd_sd.c` | `sd_config` verified (`CS 4, CLK 1, MISO 47, MOSI 2`); `sd status` reports absent card honestly | HW DEPENDENT | No microSD card inserted |
| **Infrared / NFC / External Host SDR** | `infrared_manager.c`, `desktop/serial_comm.py` | Code audited & hardened; `irpin` verified; `CONFIG_HAS_INFRARED` disabled; no host SDR attached | DISABLED / HW DEPENDENT | Non-SDR MCU; external hardware required |
| **Desktop Command Center V4** | `desktop/*.py` | PySide6 17-view GUI + Matplotlib + WAL SQLite + `GhostTelemetryParser` | PASS (`7/7` pytest) | Verified unit, DB & offscreen UI tests |
