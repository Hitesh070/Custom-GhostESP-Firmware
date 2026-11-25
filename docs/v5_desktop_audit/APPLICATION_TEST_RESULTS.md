# Application Test Results Summary

## 1. Automated Pytest Suite (`pytest -v desktop/tests/`)
- **Before V5 Fixes (Baseline):** `9 passed, 5 failed` (exposed `NoneType` crash in `temp_json`, `2.45 GHz -> 2045` math bug, stale KPIs on disconnect, missing console `Up` history, missing `matplotlib` in `requirements.txt`).
- **After V5 Fixes:** **`14 passed, 1 warning in 1.40s`** (`test_app_protocol_db.py`: 7 passed; `test_v5_exhaustive.py`: 7 passed).

## 2. Live Hardware Integration Check (`COM5` — Read-Only, Firmware Unmodified)
- **Transport:** `GhostSerialTransceiver` connected to `COM5` (`USB Serial Device`, priority `0`) @ `115200` baud (`8,870` RX bytes, `124` lines).
- **Parsed Live Payloads:** `temp_json` (`35.2 C` / `95.4 F`, `64 KB` SRAM, `8093 KB` PSRAM, `0` errors) and all `8` `profile_row` entries (`0..7`).
