# Desktop Application Inventory (GhostESP Command Center V5)

- **Application Root:** `desktop/`
- **Framework & Language:** Python 3.12 + **PySide6** (`Qt6`) + **Matplotlib** (`FigureCanvasQTAgg`) + **pyserial** + **SQLite3 (WAL)**
- **Entry Point:** [`desktop/app.py`](../../desktop/app.py) (`GhostCommandCenterWindow`, `main()`)

## Module Inventory
| File | Role |
|---|---|
| [`desktop/app.py`](../../desktop/app.py) | 17-view PySide6 GUI, Qt signal dispatch, Matplotlib charts, command routing, CSV/JSON export |
| [`desktop/parsers.py`](../../desktop/parsers.py) | ANSI stripper & stateful line parser (`GhostTelemetryParser`, `ParsedEvent`) |
| [`desktop/serial_comm.py`](../../desktop/serial_comm.py) | `GhostSerialTransceiver` (queued TX, prompt-flush RX, port priority) & `GhostSDRInterface` |
| [`desktop/database.py`](../../desktop/database.py) | Thread-safe `GhostDatabase` (7 SQLite WAL tables, session bundle & timeline JSON export) |
| [`desktop/theme.py`](../../desktop/theme.py) | Dark engineering theme palette (`PALETTE`, `APP_STYLESHEET`) |
| [`desktop/modules/sdr_abstraction.py`](../../desktop/modules/sdr_abstraction.py) | Legacy host SDR abstract base / honest `NullSDRDriver` (`detect_connected_devices()` returns `[]`) |
| [`desktop/requirements.txt`](../../desktop/requirements.txt) | `pyserial>=3.5`, `PySide6>=6.5.0`, `matplotlib>=3.8.0`, `pytest>=8.0.0` |
| [`desktop/tests/test_app_protocol_db.py`](../../desktop/tests/test_app_protocol_db.py) | 7 unit/DB/parser/offscreen-UI tests |
| [`desktop/tests/test_v5_exhaustive.py`](../../desktop/tests/test_v5_exhaustive.py) | 7 exhaustive V5 button, calculation, disconnect, history & export tests |

## 17 Views (`QStackedWidget`)
`Dashboard`, `Device & Serial`, `Hardware Pinout`, `Temperature Monitor`, `Wi-Fi Discovery`, `Rogue & Deauth`, `BLE Lab`, `NRF24 RF Spectrum`, `SDR Interface`, `IR / NFC / Sub-GHz`, `BadUSB / USB HID`, `SD & PCAP Storage`, `Profiles & Config`, `Memory & RTOS`, `Timeline & Alerts`, `Command Console`, `Help & V4 Audit`.
