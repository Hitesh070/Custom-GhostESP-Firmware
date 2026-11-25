# ESP32-S3 Internal Temperature Monitor Specification (V4)

## 1. Objective & Hardware Grounding
Provide an independent, non-blocking ESP32-S3 on-chip silicon die temperature monitor replacing the broken V3 `TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 85)` call that failed at runtime with `ESP_ERR_INVALID_ARG` (`Cannot select the correct range`).

- **Display Label:** `ESP32 CHIP INTERNAL TEMPERATURE`
- **Physical Sensor:** ESP32-S3 SAR/TSENS internal junction sensor (`SOC_TEMP_SENSOR_SUPPORTED` via `driver/temperature_sensor.h`)
- **IDF Supported Ranges (`temperature_sensor_periph.c`):**
  - Default active range: `-10 C to +80 C` (calibrated range)
  - Auto-escalation threshold 1: if reading `>= 75.0 C` -> re-installs `20 C to 100 C`
  - Auto-escalation threshold 2: if reading `>= 95.0 C` -> re-installs `50 C to 125 C`
  - Down-range fallback: if reading fails at higher band -> restores `-10 C to 80 C`

## 2. Firmware Architecture
- **Header:** [`include/modules/temp_monitor.h`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/include/modules/temp_monitor.h)
- **Implementation:** [`main/modules/temp_monitor.c`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/main/modules/temp_monitor.c)
- **Boot Init:** Called during startup in [`main/main.c`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/main/main.c) (`temp_monitor_init()`)
- **CLI Integration:** [`main/core/commands/cmd_godmode.c`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/main/core/commands/cmd_godmode.c) (`handle_temp_cmd`)
- **Concurrency & Timing:**
  - `SemaphoreHandle_t s_mutex` guards sensor handle, rolling statistics (`min_c`, `max_c`, `avg_c`, `sample_count`, `read_errors`), and `TEMP_MONITOR_HISTORY_MAX` (`120` samples) ring buffer.
  - Periodic background sampling via `esp_timer` (`ESP_TIMER_TASK`, `skip_unhandled_events = true`) with configurable interval `500 ms` to `60000 ms` (default `2000 ms`).

## 3. CLI Subcommands & Telemetry Format

| Command | Behavior |
|---|---|
| `temp` / `temp status` | Immediate fresh sample + human-readable report (`C`/`F`, Min/Max/Avg, active HW range, SRAM/PSRAM free KB, sample/error counts) |
| `temp c` / `temp f` | Switch summary min/max/avg unit to Celsius or Fahrenheit and print report |
| `temp interval <ms>` | Set periodic sampler period (`500..60000 ms`); rejects out-of-range values with explicit bounds message |
| `temp history` | Print up to 30 recent samples from the 120-entry ring buffer with ASCII bar chart |
| `temp reset` | Clear sample history buffer and min/max/avg/error statistics |
| `temp json` | Emit single-line machine-readable `[TEMP_JSON] {...}` telemetry for **GhostESP Command Center** |

### `[TEMP_JSON]` Schema
```text
[TEMP_JSON] {"sensor":"ESP32 CHIP INTERNAL TEMPERATURE","init":<bool>,"valid":<bool>,"temp_c":<float>,"temp_f":<float>,"min_c":<float>,"max_c":<float>,"avg_c":<float>,"unit":"C|F","interval_ms":<u32>,"samples":<u32>,"errors":<u32>,"last_ms":<u32>,"range_min":<int>,"range_max":<int>,"sram_free_kb":<uint>,"psram_free_kb":<uint>}
```

## 4. Safety & Calibration Notes
- Measures internal silicon junction temperature, **NOT ambient air temperature**.
- ESP32-S3 is rated up to `105 C`; continuous Wi-Fi SoftAP/scanning at 240 MHz normally yields `45-60 C`.
- Soft failure when sensor unavailable emits structured status (`valid=false`, incremented `read_errors`) rather than rebooting or hanging.
