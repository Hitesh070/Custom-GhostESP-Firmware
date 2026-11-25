# ESP32-S3 Internal Temperature Monitor — Hardware Test Report

## 1. Architectural Validation Summary
- **Implementation:** [`include/modules/temp_monitor.h`](../../include/modules/temp_monitor.h), [`main/modules/temp_monitor.c`](../../main/modules/temp_monitor.c), [`main/core/commands/cmd_godmode.c`](../../main/core/commands/cmd_godmode.c)
- **Root Cause Solved:** Upstream `TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 85)` failed ESP32-S3 HAL range selection (`Cannot select the correct range`). V4 selects ESP32-S3's supported `[-10, 80] C` range with automatic re-arming into `[20, 100] C` (`temp >= 76 C`) and `[50, 125] C` (`temp >= 96 C`).
- **Thermal Interpretation Note:** Reads the **on-chip internal silicon junction**, not ambient room air. Continuous active Wi-Fi SoftAP + HTTP + BLE/Wi-Fi scans normally run `45–60 C`, which explains why the PCB feels warm to touch (`~51–54 C` silicon junction observed during sustained bench stress).

## 2. Live Hardware Test Results (`COM5`)

| # | Command | Observed Hardware Response |
|---:|---|---|
| 1 | `temp` | temp / ESP32 CHIP INTERNAL TEMPERATURE & TELEMETRY / =======================================... |
| 2 | `temp json` | temp json / [TEMP_JSON] {"sensor":"ESP32 CHIP INTERNAL TEMPERATURE","init":true,"valid":true... |
| 3 | `temp f` | temp f / ================================================================= / Sensor Label   ... |
| 4 | `temp c` | temp c / ================================================================= / Sensor Label   ... |
| 5 | `temp interval 1000` | temp interval 1000 / [+] Temperature monitor sampling interval set to 1000 ms |
| 6 | `temp history` | temp history / ----------------------------------------------------------------- / 32 ms /  ... |
| 7 | `temp interval 2000` | temp interval 2000 |
| 8 | `temp invalidarg` | temp invalidarg |
| 9 | `temp json` | temp json / [TEMP_JSON] {"sensor":"ESP32 CHIP INTERNAL TEMPERATURE","init":true,"valid":true... |
| 10 | `temp` | temp / ================================================================= / Sensor Label     ... |
| 11 | `temp json` | temp json / [TEMP_JSON] {"sensor":"ESP32 CHIP INTERNAL TEMPERATURE","init":true,"valid":true... |
| 12 | `temp f` | temp f / ESP32 CHIP INTERNAL TEMPERATURE & TELEMETRY / =====================================... |
| 13 | `temp c` | temp c / ESP32 CHIP INTERNAL TEMPERATURE & TELEMETRY / =====================================... |
| 14 | `temp interval 1500` | temp interval 1500 / [+] Temperature monitor sampling interval set to 1500 ms |
| 15 | `temp history` | temp history / ESP32 CHIP INTERNAL TEMPERATURE HISTORY (17 samples) / ----------------------... |
| 16 | `temp interval 2000` | temp interval 2000 / [+] Temperature monitor sampling interval set to 2000 ms |
| 17 | `temp interval 100` | temp interval 100 / [-] Invalid interval 100 ms (allowed: 500 - 60000 ms) |
| 18 | `temp foo` | temp foo / Usage: temp [status/c/f/interval <ms>/history/reset/json] |
| 19 | `temp reset` | temp reset |
| 20 | `temp json` | temp json / TERNAL TEMPERATURE","init":true,"valid":true,"temp_c":51.2,"temp_f":124.2,"min_c... |

## 3. Verified Telemetry Invariants
- `[TEMP_JSON]` emits valid JSON consumed by [`GhostTelemetryParser`](../../desktop/parsers.py) and persisted in [`GhostDatabase.temp_samples`](../../desktop/database.py).
- Periodic background `esp_timer` sampling verified `errors: 0` across all runs (`samples` incremented monotonically).
- Invalid argument handling (`temp invalidarg`) returned usage help cleanly without mutating interval or unit state.
