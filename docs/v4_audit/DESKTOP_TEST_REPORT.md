# Desktop Application Test Report — GhostESP Command Center V4

## 1. Automated Test Execution
- **Suite File:** [`desktop/tests/test_app_protocol_db.py`](../../desktop/tests/test_app_protocol_db.py)
- **Framework:** `pytest` (Qt `QT_QPA_PLATFORM=offscreen` for headless GUI verification)
- **Latest Run Result:** **`7 passed, 1 warning`** (PySide6 deprecation/offscreen notice unrelated to assertions)

## 2. Verified Test Cases

| Test Function | Scope Covered | Assertion Summary | Result |
|---|---|---|---|
| `test_strip_ansi` | ANSI escape stripping | `"\x1b[38;5;36mghost\x1b[0m> scanap\r\n"` -> `"ghost> scanap"` | PASS |
| `test_parse_temp_json_and_text` | `[TEMP_JSON]` & fault tolerance | Verifies `temp_c == 46.5`, `sram_free_kb == 64`, malformed JSON returns `[]` cleanly | PASS |
| `test_parse_scanap_multiline_block` | Stateful Wi-Fi AP block parser | Accumulates `[0]`/`[1]` blocks across SSID/BSSID/RSSI/Channel/Vendor, flushes on `ghost>` | PASS |
| `test_parse_roguedetect_deauth_profiles_and_selftest` | Pipe-delimited protocol tables | Validates `roguedetect`, `deauthmon`, `profile list`, and `selftest` rows + timeline events | PASS |
| `test_parse_nrf24_sweep_and_ble` | RF sweep & BLE parser | Parses channel `37` (`2437 MHz`, `80%`) and BLE advertiser MAC `-61 dBm` | PASS |
| `test_database_wal_and_roundtrip` | `GhostDatabase` SQLite WAL | Sessions, AP upsert + rogue flag transition, BLE, temp, deauth, timeline, JSON export | PASS |
| `test_pyside6_window_offscreen_smoke` | Full GUI instantiation | Mounts `GhostCommandCenterWindow` (`17` views in `QStackedWidget`), updates KPIs & tables on serial lines | PASS |
