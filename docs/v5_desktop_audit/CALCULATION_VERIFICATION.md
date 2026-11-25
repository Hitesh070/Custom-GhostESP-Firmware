# Numerical Calculation Verification

| Calculation | Formula | Test Vectors | Expected | Actual | Result |
|---|---|---|---|---|---|
| Celsius -> Fahrenheit fallback | `F = (C * 9.0 / 5.0) + 32.0` | `-10.0, 0.0, 25.0, 52.2, 80.0` | `14.0, 32.0, 77.0, 126.0, 176.0` | Exact match | **PASS** |
| PSRAM KB -> MB | `psram_free_kb / 1024.0` | `8192 KB`, `8093 KB` | `8.0 MB`, `7.9 MB` | Exact match | **PASS** |
| NRF24 GHz -> MHz | `round(float("2." + dec) * 1000)` | `2.45 GHz`, `2.437 GHz` | `2450 MHz`, `2437 MHz` | Exact match (fixed `2045` bug) | **PASS** |
| Null / NaN / Inf Guard | `_safe_float()` (`math.isfinite`) | `None`, `NaN`, `Inf`, `"bad"` | Ignored safely (`None`), no crash | No exception raised | **PASS** |
