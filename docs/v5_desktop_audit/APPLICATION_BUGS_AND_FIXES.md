# Application Bugs & Fixes (V5 Desktop Audit)

| Bug ID | Severity | File(s) | Root Cause | Fix Applied | Verification |
|---|---|---|---|---|---|
| **V5-001** | Medium | `desktop/requirements.txt` | Listed unused `pyqtgraph` instead of `matplotlib` | Replaced with `matplotlib>=3.8.0` and `pytest>=8.0.0` | `test_honest_sdr_and_requirements_sync` PASS |
| **V5-002** | Low | `desktop/README.md` | Stale V3 Tkinter 7-tab docs | Updated to V5 17-view PySide6 architecture | Inspected |
| **V5-003** | Medium | `desktop/app.py` | `DTR/RTS Low` silent when offline; `macaddr` sent without `all`; narrow keyword log filters dropped `uuid`, `uptime`, `df`, `mem`, `wpa3check`, `airtags` | Added `_last_cmd` routing, `wifi_report_log`, `ble_log`, `profile_log`, `macaddr all`, `_hardware_reset_dtr_rts` | Offscreen UI + live `COM5` verified |
| **V5-004** | High | `desktop/app.py` | `QTableWidget.setSortingEnabled(True)` re-sorted mid-insert, leaving half-empty Wi-Fi/BLE rows | Disabled sorting during `_upsert_wifi` / `_upsert_ble` + col-0 lookup | Verified in `test_pyside6_window_offscreen_smoke` |
| **V5-005** | High | `desktop/parsers.py` | `BLE_INIT: Bluetooth MAC:` parsed timestamp `88734` as RSSI; `blescan -t` bare `MAC:` lines dropped | Ignored system log prefixes; added multi-line `bledetect`/`listadv` & bare `MAC:` parser | Verified in `test_pyside6_window_offscreen_smoke` & `test_nrf24_freq_precision_and_ble_filtering` |
| **V5-006** | Medium | `desktop/parsers.py` | `2.45 GHz` parsed as `2045 MHz` (`2000 + int("45")`) | Changed to `round(float("2." + dec) * 1000)` | `test_nrf24_freq_precision_and_ble_filtering` PASS |
| **V5-007** | Medium | `desktop/app.py` | `temp_json` `null`/`NaN`/`Inf` crashed `float(...)`; disconnect left stale live KPIs | Added `_safe_float`/`_safe_int` (`math.isfinite`) & disconnect KPI/status reset | `test_deterministic_calculations_and_null_nan_resilience` & `test_disconnect_clears_stale_live_indicators` PASS |
| **V5-008** | Medium | `desktop/app.py` | `Console` lacked `Up`/`Down` history navigation and `Clear` button; tables editable; `selftest` appended duplicates | Added `eventFilter` history, `Clear` button, `NoEditTriggers`, `selftest_table` clear | `test_console_history_clear_and_exports` PASS |
| **V5-009** | Low | `desktop/serial_comm.py` & `sdr_abstraction.py` | Prompt `ghost> ` without `\n` delayed final block flush; `NullSDRDriver` returned fake `"supported": True` | Added prompt flush in `_read_loop`; `detect_connected_devices()` returns `[]` | Code & transport verified |
