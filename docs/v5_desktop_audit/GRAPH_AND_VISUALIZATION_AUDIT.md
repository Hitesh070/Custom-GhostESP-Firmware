# Graph & Visualization Audit

1. **Silicon Die Thermal Trend (`temp_chart`, Matplotlib):**
   - **Data Source:** Valid `temp_json` / `temp_text` finite samples appended to `self.temp_points` (`deque(maxlen=120)`).
   - **Y-Axis:** Celsius (`C`).
   - **Empty / Reset State:** `Reset Stats` clears `self.temp_points` and redraws empty grid cleanly; `NaN`/`Inf` rejected by `_safe_float()`.
2. **2.4 GHz Carrier Activity (`nrf_chart`, Matplotlib):**
   - **Data Source:** `nrf24_sweep_bin` (`0..125` channels, `0..100%` activity).
   - **Fixed Limits:** `xlim(0, 125)`, `ylim(0, 100)` so partial sweeps never distort axis scale.
   - **No Fake Animation:** Charts update strictly on incoming telemetry events; zero background timers invent points.
