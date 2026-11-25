# Button & Control Audit (GhostESP Command Center V5)

All 17 navigation buttons, TopBar controls (`Port`, `Baud`, `Refresh`, `CONNECT`/`DISCONNECT`), and `60+` action buttons were exercised in [`test_every_command_button_connected_and_offline_safe`](../../desktop/tests/test_v5_exhaustive.py) under `QT_QPA_PLATFORM=offscreen`.

| Screen | Controls Verified | Backend Action | Offline Safe | Result |
|---|---|---|---|---|
| **TopBar** | `Port`, `Baud`, `Refresh`, `CONNECT`/`DISCONNECT` | `GhostSerialTransceiver.list_available_ports()`, `connect()`, `disconnect()` | Yes | **PASS** |
| **Sidebar (17)** | `01. Dashboard` .. `17. Help & V4 Audit` | `_switch_page(idx)` updates `QStackedWidget` & `[active="true"]` style | Yes | **PASS** |
| **Dashboard** | `Read Die Temp`, `Run Self-Test`, `Start Wi-Fi Scan`, `Stop Scan`, `Start BLE Detect`, `Rogue Status`, `Deauth Status`, `Profiles List` | `_send_cmd(...)` | Yes (`QMessageBox`) | **PASS** |
| **Device & Serial** | `chipinfo`, `version`, `uuid`, `macaddr` (`macaddr all`), `uptime`, `Hardware Reset (DTR/RTS Low)` | `_send_cmd(...)` / `_hardware_reset_dtr_rts()` | Yes (`QMessageBox`) | **PASS** |
| **Hardware Pinout** | `Execute Live Hardware Self-Test` | Clears `selftest_table`, sends `selftest` | Yes | **PASS** |
| **Temperature Monitor** | `Sample Now`, `Unit: C`, `Unit: F`, `Reset Stats`, `Export CSV`, `Auto-Poll` checkbox, interval `QSpinBox` | `temp json/c/f/reset`, `QTimer`, CSV write | Yes | **PASS** |
| **Wi-Fi Discovery** | `Start AP Scan`, `Stop Scan`, `List Cached APs`, `WPA3 Check`, `Channel Congestion`, `Clear Table`, `Export CSV` | `_send_cmd(...)`, `_clear_wifi()`, `_export_wifi_csv()` | Yes | **PASS** |
| **Rogue & Deauth** | `Rogue Start/Status/Stop`, `DeauthMon Start/Status/Stop` | `_send_cmd(...)` | Yes | **PASS** |
| **BLE Lab** | `BLE Detect`, `BLE Scan -t`, `BLE Scan -adv`, `List Adv`, `Stop BLE Scan`, `AirTags`, `Flippers`, `Clear`, `Export CSV` | `_send_cmd(...)`, `_clear_ble()`, `_export_ble_csv()` | Yes | **PASS** |
| **NRF24 RF Spectrum** | `Inspect Registers`, `126-Ch Sweep`, `SPI2 Echo Diagnostic` | `nrf24lab diag/sweep 10/benchmark 100` | Yes | **PASS** |
| **IR / NFC / Sub-GHz** | `Check IR Pin Config`, `Run NFC Self-Test`, `GPS Info` | `irpin`, `nfctest`, `gpsinfo -s` | Yes | **PASS** |
| **BadUSB / USB HID** | `--confirm` checkbox, `BadUSB List`, `USB Kbd Status`, `Run Builtin BadUSB`, `Stop BadUSB` | Blocks `badusb run builtin --confirm` unless checkbox is checked | Yes | **PASS** |
| **SD & PCAP Storage** | `SD Config`, `List SD Root`, `Capture List`, `Stop Capture` | `sd_config`, `sd list`, `capture -list/-stop` | Yes | **PASS** |
| **Profiles & Config** | `Refresh Profiles`, `Apply 0/1/2/3/6` | `profile list`, `_apply_profile(id)` + auto-refresh `profile list` | Yes | **PASS** |
| **Memory & RTOS** | `Memory (mem)`, `Tasks (ps)`, `Filesystems (df)`, `Coredump` | `_send_cmd(...)` routed to `health_log` | Yes | **PASS** |
| **Timeline & Alerts** | `Clear View`, `Export Timeline JSON`, `Export Full Session Bundle` | `timeline_table.setRowCount(0)`, `GhostDatabase` exports | Yes | **PASS** |
| **Command Console** | `QLineEdit` (`QCompleter`, `Up`/`Down` history), `SEND`, `Clear` | `_send_console_input()`, `console_out.clear()` | Yes | **PASS** |
| **Help & V4 Audit** | `help`, `help godmode`, `help wifi`, `help ble`, `ghost version` | `_send_cmd(...)` routed to `help_log` | Yes | **PASS** |
