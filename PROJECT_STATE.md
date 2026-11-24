# Custom GhostESP Firmware – Complete Project Context & Status

**Project:** Custom GhostESP Firmware on ESP32-S3  
**Repository:** [Custom-GhostESP-Firmware](https://github.com/Hitesh070/Custom-GhostESP-Firmware)  
**Local Project Path:** `C:\Users\hites\OneDrive\Documents\Projects\Custom GhostESP Firmware`  
**Reference Repo Path:** `C:\Users\hites\OneDrive\Documents\ESP32\GhostESP`  
**Current Release Version:** `v1.1.0`  
**Date:** November 24, 2026  

---

## 1. Hardware Specifications

| Component | Detail | Notes |
| :--- | :--- | :--- |
| **Development Board** | 7Semi ESP32-S3-WROOM-1-N16R8 | Dual USB-C ports |
| **Microcontroller** | ESP32-S3 (Xtensa Dual-Core LX7 @ 240 MHz + LP core) | QFN56 revision v0.2 |
| **Flash Memory** | 16 MB SPI Flash | Configured for DIO @ 80 MHz |
| **PSRAM** | 8 MB Octal PSRAM | AP_3v3 embedded |
| **Onboard RGB LED** | 1 × WS2812B NeoPixel on **GPIO48** | GRB byte order, 800 kHz RMT driver |
| **Display Configuration** | **1.8" ST7735S 128×160 SPI TFT** | SPI3 bus: MOSI=15, SCK=16, CS=5, DC=6, RST=7, BCKL=3.3V (Active), VCC=5V, GND=GND |
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

## 7. NRF24L01+PA+LNA Hardware Integration & Pinout (PERMANENT CONFIGURATION)

The NRF24L01+PA+LNA transceiver with 8-pin 3.3V AMS1117 socket adapter is officially integrated. The pins map to a continuous 7-pin block located at the bottom-left of the 7Semi ESP32-S3 header.

### A. Pin Connection Table

| NRF24 Adapter Pin | Function | 7Semi ESP32-S3 Pin | Header Location & Board Label | Wire Recommendation |
| :--- | :--- | :--- | :--- | :--- |
| **GND** | Ground | **GND** | Left Header, Bottom Pin (Pin 22) | Black jumper wire |
| **VCC** | Power In (5V) | **5V (VIN)** | Left Header, Pin 21 (Red 5V/VIN) | Red jumper wire (Powers AMS1117 regulator) |
| **CE** | Chip Enable | **GPIO 14** | Left Header, Pin 20 (Touch 14 / ADC2 CH3) | Orange/White wire |
| **CSN** | SPI Chip Select | **GPIO 10** | Left Header, Pin 16 (Default `SPI_CS`) | Yellow wire |
| **SCK** | SPI Clock | **GPIO 12** | Left Header, Pin 18 (Default `SPI_SCK`) | Green wire |
| **MOSI**| SPI Master Out | **GPIO 11** | Left Header, Pin 17 (Default `SPI_MOSI`) | Blue wire |
| **MISO**| SPI Master In | **GPIO 13** | Left Header, Pin 19 (Default `SPI_MISO`) | Purple wire |
| **IRQ** | Interrupt | **NC (Not Connected)**| Not needed (driver uses SPI register polling) | Leave disconnected |

> [!IMPORTANT]
> **Power Supply Rule:** Always connect the socket adapter VCC to the **5V / VIN** pin on the ESP32-S3. The socket adapter includes an onboard AMS1117 3.3V LDO regulator that regulates 5V down to 3.3V with the necessary current capacity (up to 250 mA peak during PA transmission bursts). Connecting the adapter to the 3.3V pin can cause brownouts or radio failures.

### B. Firmware Configuration
- **SPI Host:** `SPI2_HOST` (FSPI / SPI2 peripheral)
- **Clock Speed:** 8 MHz (`CONFIG_NRF24_SPI_CLOCK_HZ=8000000`)
- **Verified CLI Commands:**
  - `nrf24 status` $\to$ Reports current state and verifies `SPI2 MOSI=11 MISO=13 SCK=12 CSN=10 CE=14`
  - `nrf24 start` $\to$ Starts 2.4 GHz spectrum analyzer sweep
  - `nrf24 stop` $\to$ Stops analyzer task and frees SPI bus

---

## 8. Soldering & Pin Electrical Diagnostic (Verified November 23, 2026)

All 25 exposed breakout pins on both headers were verified with custom diagnostic firmware:
- **Left Header (16 Pins):** GPIO 4, 5, 6, 7, 15, 16, 17, 18, 8, 3, 9, 10, 11, 12, 13, 14 $\to$ **100% PASS**
- **Right Header (9 Pins):** GPIO 1, 2, 42, 41, 40, 39, 38, 21, 47 $\to$ **100% PASS**
- **Solder Bridge Detection:** No short circuits to Ground, 3.3V, or adjacent pins detected across the entire board.
- **PSRAM Subsystem:** 8 MB Octal PSRAM passed 1 MB dynamic allocation and pattern verification (`0xAA55AA55`).

---

## 9. 1.8" ST7735S 128×160 SPI TFT Display Integration & GUI Scaling (Verified November 24, 2026)

### A. Pin Wiring Table
The 1.8-inch ST7735S 128×160 red SPI TFT display communicates over **SPI3_HOST** (FSPI/SPI3 peripheral), leaving **SPI2_HOST** dedicated to the NRF24L01+ transceiver without bus contention.

| Display Header Pin | Function | 7Semi ESP32-S3 Pin | Notes |
| :--- | :--- | :--- | :--- |
| **VCC** | Power (5V/3.3V) | **5V (VIN)** | Powers onboard 3.3V LDO (`J1` open) |
| **GND** | Ground | **GND** | System common ground |
| **CS** | Chip Select | **GPIO 5** | Left Header, Pin 3 |
| **RESET** | Hardware Reset | **GPIO 7** | Left Header, Pin 5 |
| **A0 / DC** | Data / Command | **GPIO 6** | Left Header, Pin 4 |
| **SDA / MOSI** | SPI Data In | **GPIO 15** | Left Header, Pin 6 |
| **SCK / SCL** | SPI Clock | **GPIO 16** | Left Header, Pin 7 |
| **LED** | Backlight Control | **3.3V** | Tied to 3.3V for full brightness (`BCKL=-1`) |

### B. Firmware Configuration & UI Optimization
1. **SDK Configuration (`configs/sdkconfig.default.esp32s3`):**
   - `CONFIG_WITH_SCREEN=y`
   - `CONFIG_TFT_WIDTH=128`, `CONFIG_TFT_HEIGHT=160`
   - `CONFIG_LV_TFT_DISPLAY_CONTROLLER_ST7735S=y`
   - `CONFIG_LV_TFT_DISPLAY_SPI3_HOST=y`
   - `CONFIG_LV_TOUCH_CONTROLLER_NONE=y`
   - `CONFIG_TINYUSB_HID_COUNT=3` (Enables BadUSB keyboard, mouse, gamepad injection)
2. **Compact GUI Engine Scaling:**
   - Sized design tokens (`include/gui/design_tokens.h`) for screens $\le 160\text{ px}$: `GUI_GRID=2`, `GUI_STATUS_BAR_H=22`, `GUI_CONTROL_H=24`, and list padding `2 px`.
   - Dynamic font selection (`main/gui/accessibility_fonts.c`): selects Montserrat 8 / 10 fonts to prevent text truncation on compact displays.
   - Default Settings (`main/managers/settings_manager.c`): initializes small font (0) and compact row height (0).
3. **NRF24 Frequency Analyzer View (`main/managers/views/nrf24_analyzer_view.c`):**
   - Sized 80 px spectrum plot with compact `2.40G` / `2.52G` non-overlapping axis labels.
   - Dual 58 px side-by-side control buttons (**Pause** and **Back**) within 128 px width.
   - Improved RF receiver dwell sampling: keeps CE asserted during sweep, boosts transient signal hit register to min 25%, and sweeps 18 channels per tick.
4. **Build System Fix:**
   - Unquoted `-L` library path in `components/esp_audio_codec/CMakeLists.txt` commented out to allow building in directories with spaces in their path.

