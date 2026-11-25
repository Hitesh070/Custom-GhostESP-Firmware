# GhostESP Command Center V5 — Desktop Suite

The **GhostESP Command Center** is a PySide6 + Matplotlib + SQLite WAL desktop engineering suite for the **7Semi ESP32-S3-WROOM-1-N16R8** GhostESP firmware.

## Quick Start
```powershell
pip install -r desktop/requirements.txt
python desktop/app.py
```

## Automated Tests
```powershell
pytest -v desktop/tests/
```

## Architecture
- `app.py` — 17-view PySide6 GUI, Matplotlib live thermal & NRF24 spectrum charts, command-context routing, CSV/JSON export.
- `parsers.py` — Stateful ANSI-safe serial telemetry parser (`[TEMP_JSON]`, `scanap`/`list -a`, `bledetect`/`blescan -t`, `roguedetect`, `deauthmon`, `selftest`, `profile list`, `nrf24lab sweep`).
- `serial_comm.py` — Thread-safe `GhostSerialTransceiver` with queued TX, prompt-flush RX, and honest `GhostSDRInterface` host check.
- `database.py` — Thread-safe 7-table SQLite WAL store (`ghost_telemetry.db`).
- `theme.py` — Dark engineering palette & Qt stylesheet.
