# Custom GhostESP Firmware – Complete Project Context & Status

**Project:** Custom GhostESP Firmware on ESP32-S3  
**Repository:** [Custom-GhostESP-Firmware](https://github.com/Hitesh070/Custom-GhostESP-Firmware)  
**Local Project Path:** `C:\Users\hites\OneDrive\Documents\Projects\Custom GhostESP Firmware`  
**Reference Repo Path:** `C:\Users\hites\OneDrive\Documents\ESP32\GhostESP`  
**Current Release Version:** `v1.0.0`  
**Date:** November 23, 2026  

---

## 1. Hardware Specifications

| Component | Detail | Notes |
| :--- | :--- | :--- |
| **Development Board** | 7Semi ESP32-S3-WROOM-1-N16R8 | Dual USB-C ports |
| **Microcontroller** | ESP32-S3 (Xtensa Dual-Core LX7 @ 240 MHz + LP core) | QFN56 revision v0.2 |
| **Flash Memory** | 16 MB SPI Flash | Configured for DIO @ 80 MHz |
| **PSRAM** | 8 MB Octal PSRAM | AP_3v3 embedded |
| **Onboard RGB LED** | 1 × WS2812B NeoPixel on **GPIO48** | GRB byte order, 800 kHz RMT driver |
| **Display Configuration** | **Headless** (No physical LCD/OLED attached) | GUI/LVGL layers inactive |
| **USB Interfaces** | Native USB-Serial/JTAG on **COM5** (Flashing & primary CLI)<br>CP210x USB-to-UART on **COM6** (Secondary serial UART) | 115200 baud |
| **Planned Module** | **NRF24L01+PA+LNA** via 8-pin socket adapter (3.3V LDO regulated) | 2.4 GHz RF operations (Next Phase) |

---

## 2. Toolchain & Environment

- **Operating System:** Windows 11
- **Framework:** ESP-IDF v6.1.1 (`C:\esp\v6.1.1\esp-idf`)
- **Python Virtual Environment:** `C:\Espressif\tools\python\v6.1.1\venv`
- **Compiler / Toolchain:** `xtensa-esp-elf` (ESP-IDF 6.1)
- **Flashing Tool:** `idf.py -p COM5 flash` / `esptool.py v5.6.0` (460800 baud)
- **Git User:** `Hitesh070` (`hiteshvs616@gmail.com`)
- **Git Remote:** `origin` $\to$ `https://github.com/Hitesh070/Custom-GhostESP-Firmware.git`
- **Upstream Remote:** `upstream` $\to$ `https://github.com/GhostESP-Revival/GhostESP.git`

---

## 3. Issues Diagnosed & Root Causes

### 1. Knight Rider Effect Freezing on Static Red
- **Symptom:** Running `rgbmode knight` produced only a motionless solid red LED.
- **Root Cause:** The original implementation was written for multi-pixel LED strips where an index `pos` moves from `0` to `num_leds - 1`. On this 1-LED board (`num_leds == 1`), `pos` was clamped to `0` every single loop iteration, freezing the animation at the first pixel.

### 2. Police and Strobe Modes Crashing / Failing to Run
- **Symptom:** `rgbmode police` and `rgbmode strobe` failed to show any LED activity.
- **Root Cause:** Both commands checked `epilepsy_warning_enabled`. If true, they called `error_popup_create("EPILEPSY WARNING\nRapid flashing lights")`. Because the board is headless without an active display, `display_manager_run_on_lvgl()` executed synchronously and called `lv_layer_top()`. Since `lv_disp_get_default()` is `NULL`, dereferencing the display structure caused an immediate `LoadProhibited` null-pointer kernel panic/reboot before the effect tasks could ever start.
- **Secondary Cause:** Task stack sizes were set to 2048 bytes with no error handling on `xTaskCreate`.

### 3. Rainbow Brightness Scaling Not Working
- **Symptom:** `setneopixelbrightness <value>` worked for static colors and Knight Rider, but had no effect on `rgbmode rainbow` (stayed at 100% full intensity).
- **Root Cause:** While the multi-pixel matrix function `rgb_manager_rainbow_effect_matrix()` scaled 12-bit colors with `rgb_helper_scale_brightness_12bit()`, the single-LED path `rgb_manager_rainbow_effect()` completely omitted the scaling call, feeding raw 4095 (100%) values directly into gamma conversion.

### 4. Task Deadlocks & Residual Colors
- **Symptom:** Switching between modes could cause serial console hangs or leave residual static colors on after stopping an effect.
- **Root Cause:** An indefinite `while (rgb_effect_task_handle != NULL)` wait loop in `cmd_rgb.c` could block the serial task if an effect delayed termination. Additionally, task cleanup handlers were re-calling `rgb_manager_apply_static_from_settings()`.

---

## 4. Code Fixes & Architecture Changes Applied

### A. `main/core/commands/cmd_rgb.c`
1. **Headless Display Safety Guard:**
   - Wrapped `error_popup_create()` inside `if (display_manager_is_available())`.
   - On headless boards, the warning is safely logged to serial via `glog("WARNING: Epilepsy risk - rapid flashing lights\n")` without touching LVGL memory.
2. **Task Stack Allocations:**
   - Increased task stack size for `rainbow_task`, `police_task`, `strobe_task`, and `knightrider_task` from 2048 to **4096 bytes**.
   - Added validation: `if (res == pdPASS)` logging confirmation or error status.
3. **Safe Effect Switching Timeout:**
   - Replaced indefinite while-loop with a 1000 ms timeout check to eliminate serial CLI deadlocks.
4. **State Persistence on Exit:**
   - Updated `rgbmode off` to persist `RGB_MODE_NORMAL` to settings and immediately clear the LED strip.

### B. `main/managers/rgb_manager.c`
1. **Single-LED Knight Rider Cylon Eye:**
   - Added dedicated branch for `num_leds == 1` utilizing a smooth sine-wave breathing curve (`0.04` to `1.0` intensity factor), gamma correction, and live brightness scaling.
2. **High-Visibility Police Siren Animation:**
   - Implemented `set_police_strip_colors()`: 3 rapid Red flashes (45 ms ON, 40 ms OFF) $\to$ 120 ms pause $\to$ 3 rapid Blue flashes (45 ms ON, 40 ms OFF) $\to$ 120 ms pause.
3. **High-Frequency Strobe Animation:**
   - Implemented `set_all_strip_color()`: Crisp 10 Hz white strobe flashes (35 ms ON, 65 ms OFF).
4. **Rainbow Brightness Scaling:**
   - Added `rgb_helper_scale_brightness_12bit(&red, &green, &blue, 255, max_bright)` into `rgb_manager_rainbow_effect()`, reading `settings_get_neopixel_max_brightness(&G_Settings)` every frame for instant, real-time brightness adjustment.
5. **Clean Termination:**
   - Guaranteed `led_strip_clear()` and `led_strip_refresh()` are called on task exit.

### C. `configs/sdkconfig.default.esp32s3`
- Configured target for ESP32-S3 N16R8: 16 MB Flash (DIO @ 80 MHz), 8 MB Octal PSRAM, FreeRTOS 1000 Hz tick rate, and RMT WS2812 driver.

---

## 5. Verification Checklist & Current Status

| Test Case | Command | Result |
| :--- | :--- | :--- |
| **Set Max Brightness** | `setneopixelbrightness 100` | **PASS** (100% full scale confirmed) |
| **Dim Brightness** | `setneopixelbrightness 20` | **PASS** (Immediate real-time dimming confirmed) |
| **Rainbow Mode** | `rgbmode rainbow` | **PASS** (Smooth cycling spectrum + live brightness control confirmed) |
| **Knight Rider Mode** | `rgbmode knight` | **PASS** (Smooth sine breathing red Cylon eye confirmed) |
| **Police Siren Mode** | `rgbmode police` | **PASS** (3 red bursts $\to$ pause $\to$ 3 blue bursts, no LVGL crash) |
| **Strobe Mode** | `rgbmode strobe` | **PASS** (Rapid white strobe blinking, no LVGL crash) |
| **Static Colors** | `rgbmode red`, `green`, `blue` | **PASS** (Clean primary colors confirmed) |
| **Turn Off** | `rgbmode off` | **PASS** (LED clears immediately, normal mode saved) |
| **Live Switching** | e.g. `police` $\to$ `knight` $\to$ `strobe` | **PASS** (Seamless transition without rebooting) |

---

## 6. Repository & Version Control Protocol

- **New Working Project Directory:** `C:\Users\hites\OneDrive\Documents\Projects\Custom GhostESP Firmware`
- **GitHub Origin:** `https://github.com/Hitesh070/Custom-GhostESP-Firmware.git`
- **Branch:** `main`
- **Current Tag:** `v1.0.0`
- **Documentation References:**
  - Complete CLI Commands Guide: [`COMMANDS_REFERENCE.md`](file:///C:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/COMMANDS_REFERENCE.md) (All registered commands, options, and syntax examples)

- **Protocol for Future Work:**
  1. Every firmware fix/feature is developed in `Documents/Projects/Custom GhostESP Firmware`.
  2. Code is compiled (`idf.py build`) and flashed (`idf.py -p COM5 flash`).
  3. Verified on hardware over serial.
  4. Changes are committed with semantic commit messages.
  5. Version tags (`v1.0.1`, `v1.1.0`, etc.) are created and pushed to GitHub (`git push -u origin <branch> --tags`).

---

## 7. Next Phase: NRF24L01+PA+LNA Integration

With the core board hardware and RGB subsystem fully stabilized, the next phase is integrating the **NRF24L01+PA+LNA** wireless transceiver module using an 8-pin socket adapter (featuring onboard 3.3V AMS1117 regulator).

### Upcoming Work Items:
1. **SPI Pin Mapping:** Assign dedicated hardware SPI pins (SCK, MOSI, MISO, CE, CSN, IRQ) on the ESP32-S3 avoiding strapped pins (GPIO0, GPIO45, GPIO46) and native USB/JTAG (GPIO19, GPIO20) and NeoPixel (GPIO48).
2. **Power & Decoupling:** Connect socket adapter to 5V rail (adapter steps down to 3.3V) and verify a 10 µF–100 µF capacitor across VCC/GND for peak PA transmission stability.
3. **Firmware SPI Configuration:** Verify or customize NRF24 pin defines in `main/core/commands/cmd_nrf24.c` and sdkconfig.
4. **RF Testing:** Test 2.4 GHz spectrum scanning, jamming, BLE beacon advertising, and carrier wave output.
