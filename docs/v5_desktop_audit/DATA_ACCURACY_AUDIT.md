# Data Accuracy Audit (GhostESP Command Center V5)

| Displayed Metric | UI Location | Source Field | Transformation | Units | Verification | Result |
|---|---|---|---|---|---|---|
| Silicon Die Temp | Dashboard KPI & Temp Monitor | `[TEMP_JSON].temp_c` / `temp_text` | `_safe_float()` finite check, `:.1f` | `C` | Live `COM5` (`35.2 C`) + deterministic vectors | **PASS** |
| Silicon Die Temp (F) | Temp Monitor `temp_now_lbl` | `[TEMP_JSON].temp_f` (fallback `(C*9/5)+32`) | `_safe_float()`, `:.1f` | `F` | Live `COM5` (`95.4 F`) + formula verified | **PASS** |
| Temp Min / Max / Avg | Temp Monitor `temp_stats_lbl` | `[TEMP_JSON].min_c/max_c/avg_c` | `_safe_float()`, `:.1f` | `C` | Live `COM5` (`33.2 / 35.2 / 34.2 C`) | **PASS** |
| Internal SRAM Free | Dashboard KPI & Memory | `[TEMP_JSON].sram_free_kb` | `_safe_int()`, `>= 0` | `KB` | Live `COM5` (`64 KB`) | **PASS** |
| Octal PSRAM Free | Dashboard KPI | `[TEMP_JSON].psram_free_kb` | `psram_free_kb / 1024.0` (`:.1f`) | `MB` | `8192 KB -> 8.0 MB`, live `8093 KB` | **PASS** |
| Discovered APs | Dashboard KPI & Wi-Fi Table | `wifi_ap` (`scanap`/`list -a`/`roguedetect`) | `len(self.known_aps)` | count | Sorting-safe `_upsert_wifi` verified | **PASS** |
| BLE Advertisers | Dashboard KPI & BLE Table | `ble_device` (`bledetect`/`blescan`/`listadv`) | `len(self.known_bles)` | count | `BLE_INIT:` excluded; `blescan -t` parsed | **PASS** |
| Rogue / Deauth Alerts | Dashboard KPI & `sec_summary` | `deauth_summary` (`total_deauths`, `rate_fps`) | Direct integer display | count / fps | Deterministic mock + disconnect reset | **PASS** |
| NRF24 Sweep Activity | NRF24 Chart | `nrf24_sweep_bin` (`channel`, `freq_mhz`, `activity_pct`) | `round(float("2." + dec) * 1000)` | `MHz` / `%` | `2.45 GHz -> 2450 MHz`, `2.437 GHz -> 2437 MHz` | **PASS** |
| Host SDR Status | SDR Interface | `GhostSDRInterface.get_info()` | Checks `rtl_power`/`hackrf_sweep` in `PATH` | text | Honest `"ESP32-S3 is NOT an SDR"` | **PASS** |

- **No Fabricated Telemetry Check:** Inspected all files in `desktop/`. Fixed `NullSDRDriver.detect_connected_devices()` in [`sdr_abstraction.py`](../../desktop/modules/sdr_abstraction.py) to return `[]` instead of a placeholder dict.
