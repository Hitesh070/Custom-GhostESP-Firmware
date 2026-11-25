# Desktop Application Architecture (`GhostESP Command Center V4`)

## 1. Overview
Upgraded the V3 Tkinter prototype in [`desktop/app.py`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/desktop/app.py) to a modular, industry-grade **PySide6 + Matplotlib + SQLite WAL** engineering suite with 17 structured navigation views.

## 2. Module Breakdown
| Module | Responsibility |
|---|---|
| [`desktop/theme.py`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/desktop/theme.py) | Dark engineering design system (`PALETTE`, `APP_STYLESHEET`) with high-contrast status badges (`LIVE`, `OFFLINE`, `SIMULATED / HW REQUIRED`) |
| [`desktop/parsers.py`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/desktop/parsers.py) | ANSI stripper and incremental `GhostTelemetryParser` covering `[TEMP_JSON]`, `temp` text, `scanap` multi-line AP blocks, `roguedetect`, `deauthmon`, `bledetect`/`blescan`, `nrf24lab sweep/diag`, `profile list`, `selftest`, and `mem` |
| [`desktop/serial_comm.py`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/desktop/serial_comm.py) | Thread-safe `GhostSerialTransceiver` (`threading.Lock`, queued TX writer + flush, RX line decoder, configurable baudrate & DTR/RTS controls, ESP32-S3 port prioritization) and honest `GhostSDRInterface` |
| [`desktop/database.py`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/desktop/database.py) | Thread-safe SQLite store (`PRAGMA journal_mode=WAL`) managing `sessions`, `wifi_aps`, `ble_devices`, `deauth_events`, `temp_samples`, `nrf24_scans`, and `event_timeline`, with CSV/JSON exports |
| [`desktop/app.py`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/desktop/app.py) | `GhostCommandCenterWindow` Qt6 main window bridging worker threads via `SerialSignals` (`Signal`), 17 sidebar views, Matplotlib die-temperature & NRF24 spectrum charts, command autocomplete, and explicit `--confirm` BadUSB safety guard |
| [`desktop/tests/test_app_protocol_db.py`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/desktop/tests/test_app_protocol_db.py) | 7 pytest tests verifying ANSI stripping, malformed/valid `[TEMP_JSON]`, multi-line `scanap`, `roguedetect`, `deauthmon`, `nrf24lab`, SQLite WAL persistence, and offscreen PySide6 GUI state updates |

## 3. Navigation Architecture (17 Views)
1. Dashboard (KPI cards + live operational feed)
2. Device & Serial (`chipinfo`, `version`, `uuid`, `macaddr`, `uptime`, DTR/RTS)
3. Hardware Pinout (Verified GPIO authority + `selftest` table)
4. Temperature Monitor (`temp json` auto-poller + Matplotlib C/F trend chart + CSV export)
5. Wi-Fi Discovery (`scanap`, `stopscan`, `list -a`, `wpa3check`, `congestion` + sortable AP table)
6. Rogue & Deauth (`roguedetect` & `deauthmon` controls + deauth frame table)
7. BLE Lab (`bledetect`, `blescan -t`, `stopscan`, `listairtags`, `listflippers` + table)
8. NRF24 RF Spectrum (`nrf24lab diag/sweep/benchmark` + Matplotlib 126-ch chart + HW badge)
9. SDR Interface (Explicit external host SDR abstraction & disclaimer)
10. IR / NFC / Sub-GHz (HW-dependent controls)
11. BadUSB / USB HID (Requires explicit `--confirm` checkbox + USB-OTG PHY CDC warning)
12. SD & PCAP Storage (`sd_config`, `sd list`, `capture -list`)
13. Profiles & Config (Interactive 0-7 profile switcher + table)
14. Memory & RTOS (`mem`, `ps`, `df`, `coredump`)
15. Timeline & Alerts (SQLite-backed timeline + JSON/Bundle exports)
16. Command Console (Autocomplete `QCompleter` + ANSI-clean terminal)
17. Help & V4 Audit (Reference shortcuts)
