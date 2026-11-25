# Desktop UI/UX Engineering Audit (`desktop/`)

## 1. V3 Problems Identified & Resolved
- **V3 Baseline Failures (Confirmed in User Screenshots):** Static/empty `BLE Lab`, empty `Timeline`, fragile regexes missing multi-line `scanap` and pipe-delimited output, blocking serial I/O, and lack of persistent session database.
- **V4 Resolution:** Re-engineered into modular PySide6 architecture ([`app.py`](../../desktop/app.py), [`theme.py`](../../desktop/theme.py), [`parsers.py`](../../desktop/parsers.py), [`serial_comm.py`](../../desktop/serial_comm.py), [`database.py`](../../desktop/database.py)).

## 2. Navigation & Information Architecture (17 Views)
1. **Dashboard** (6 live KPI cards + quick safe actions)
2. **Device & Serial** (COM port priority sorting, DTR/RTS controls, RX/TX counters)
3. **Hardware Pinout** (`PINOUT_ROWS` canonical 7Semi ESP32-S3 table)
4. **Temperature Monitor** (live Matplotlib trend chart + C/F + SRAM/PSRAM readout)
5. **Wi-Fi Discovery** (`scanap`, `stopscan`, `list -a`, `wpa3check`, `congestion`, CSV export)
6. **Rogue & Deauth** (`roguedetect` & `deauthmon` live status, reason codes)
7. **BLE Lab** (`bledetect`, `blescan`, `listairtags`, `listflippers`, live sortable table)
8. **NRF24 RF Spectrum** (126-ch Matplotlib spectrum bar/plot + honest `HW DEPENDENT` state)
9. **SDR Interface** (host `rtl_power`/`hackrf_sweep` probe — never fakes ESP32 SDR)
10. **IR / NFC / Sub-GHz** (`irpin`, `nfctest`, honest Kconfig/HW state badges)
11. **BadUSB / USB HID** (explicit `--confirm` guard dialog + `usbkbd status`)
12. **SD & PCAP Storage** (`sd_config`, `sd status`, `sd list`)
13. **Profiles & Config** (`profile list` table + `profile apply <id>`)
14. **Memory & RTOS** (`mem`, `ps`, `df`, `selftest` table)
15. **Timeline & Alerts** (color-coded severity events + JSON export)
16. **Command Console** (`QCompleter` autocomplete over `COMMON_COMMANDS` + history)
17. **Help & V4 Audit** (integrated audit status & command categories)
