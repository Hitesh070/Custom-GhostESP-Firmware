# IR, Peripherals & Sensor Specialist Audit Report
**Project:** Custom GhostESP Firmware (GhostESP God Mode V2)  
**Agent:** AGENT 4 - IR, Peripherals & Sensor Specialist  
**Target Architecture:** ESP32, ESP32-S3, ESP32-C5, ESP32-P4  
**Date:** November 2026  

---

## 1. Executive Summary

This audit and optimization report details the comprehensive review, safety remediation, and modular enhancements implemented across the **Infrared (IR) Subsystem**, **SD Card Storage Manager**, and **External Peripheral Interfaces (CC1101 SubGHz, LoRa SX1262, SPI Display multiplexing)** in the Custom GhostESP Firmware.

### Key Issues Identified & Resolved:
1. **Critical Hardware Safety Flaw in IR Transmission:** Prior code actively asserted `gpio_set_level(infrared_get_tx_pin(), 1)` (continuous DC HIGH) before protocol encoding and RMT carrier transmission began. In transistor/MOSFET-driven IR blasters, this injected heavy DC current (often 200mA–1A) through the LED/transistor for milliseconds, creating severe thermal burnout hazards and system brownouts. This has been remediated with strict idle-low enforcement.
2. **Universal IR Header Memory Duplication & Broken CLI Command:** `include/core/universal_ir.h` previously defined a 61-element array statically inside the header file, duplicating the table into every translation unit that included it. Furthermore, all signals were hardcoded with the name `"POWER"`, while `cmd_ir.c` searched for `"Power Off"`, causing `ir universals sendall TURNHISTVOFF` to silently transmit 0 signals. The array has been decoupled into `universal_ir.c`, expanded with universal controls (MUTE, VOL+, VOL-, CH+, CH-, INPUT), and backed by fuzzy action matching.
3. **SD Card Manager Hardcoded Pins & Inverted Pullup Configuration:** In SDMMC 1-bit mode, configured pins were completely ignored in favor of compile-time constants, and GPIO pull-up comments/assignments were inverted between D0, CLK, and CMD. Hardcoded defaults (GPIO 19, 18, 20...) were assigned even when board configurations differed.
4. **Outdated GPIO Pin Boundaries in SD CLI:** Commands in `cmd_sd.c` artificially rejected pins `> 40`, breaking custom pin mapping on ESP32-S3 (which supports GPIO 0–48).
5. **Lack of Lifecycle and Health Status Tracking:** Neither the IR subsystem nor the SD card manager provided a clear health check or runtime mount/unmount interface. A full lifecycle state machine (`sd_card_status_t`) and diagnostic commands (`ir raw`, `ir info`, `ir test`, `sd mount`, `sd unmount`) have been integrated.

---

## 2. Infrared (IR) Framework Audit & Optimizations

### 2.1 Protocol Detection & Decoding Engine
GhostESP incorporates a multi-layer IR stack supporting both raw timings and high-level protocol decoding:
- **Supported Protocols:** NEC, NEC Extended (`necext`), NEC42, NEC42 Extended, Samsung32, Sony SIRC (12, 15, and 20-bit), Philips RC5, RC5X, RC6 (Mode 0), Kaseikyo (Panasonic 48-bit), RCA (24-bit), and Pioneer (32-bit).
- **Decoder Architecture (`infrared_decoder.c`):** Utilizes protocol specifications with configurable preambles, mark/space bit lengths, tolerances, and split times. Decoding mechanisms include Pulse Distance Width Modulation (PDWM) and Manchester encoding (starting from mark or space).
- **Receiver Architecture (`infrared_rx_gpio.c`):** Utilizes a GPIO interrupt edge-capture architecture backed by hardware `esp_timer` (2 µs tick resolution, 15 ms timeout) to capture raw transitions with zero dropped pulses, bypassing RMT RX FIFO constraints on ESP32-S3 while maintaining precision.

### 2.2 RMT Driver Configuration & ESP32-S3 Compatibility
- **RMT API Compatibility:** Built on the ESP-IDF v5/v6 RMT driver (`rmt_tx.h`, `rmt_new_tx_channel`, `rmt_apply_carrier`, `rmt_new_copy_encoder`, `rmt_transmit`).
- **Memory Block Allocation:** On ESP32-S3, `SOC_RMT_MEM_WORDS_PER_CHANNEL` is 48 symbols. The transmit queue depth was increased from `1` to `4` to prevent FIFO starvation and transaction drops during multi-packet raw sequences or high FreeRTOS task loads.
- **Dynamic Pin Re-targeting:** `send_rmt()` now monitors pin configuration changes (`s_last_tx_pin`). If a user reconfigures the IR TX pin via NVS settings (`irpin tx <pin>`), the channel and copy encoder are dynamically released and rebuilt on the newly selected GPIO without requiring a hard power cycle.

### 2.3 Hardware Safety: Transistor & MOSFET Driving Rules
IR LEDs require short, high-energy pulses to achieve range (often 10–30 meters). In typical blaster hardware:
- **Pulsed Current:** Carrier frequencies (36 kHz–56 kHz, standard 38 kHz) operate with 25%–50% duty cycles (typical 33%). At 33% duty cycle, peak LED currents can safely reach 200 mA to 1.5 A through an NPN/NMOS driver transistor.
- **Continuous DC Hazard:** If the driving GPIO is held at continuous HIGH (DC), current limiting is only governed by the minimal series resistor (often 2–10 Ω). Within 50–200 ms, the LED undergoes thermal runaway, the driver transistor exceeds its Safe Operating Area (SOA), and the 3.3V/5V supply rail collapses, triggering an ESP32 brownout detector reset (`ESP_RST_BROWNOUT`).
- **Remediation:** Removed erroneous `gpio_set_level(infrared_get_tx_pin(), 1)` in `infrared_manager_transmit()`. All transmitters are maintained at active low/idle 0 (`flags.polarity_active_low = false`), with carrier modulation generated purely by hardware RMT.

### 2.4 Universal IR Signal Database Modularization
- **Decoupled Architecture:** Removed the static table definition from `include/core/universal_ir.h`. The database is now `extern const universal_ir_signal_t universal_ir_signals[]` defined in `main/core/universal_ir.c`, saving program memory across multiple translation units.
- **Categorized Actions:** Added `universal_ir_action_t` (POWER, MUTE, VOL_UP, VOL_DOWN, CH_UP, CH_DOWN, INPUT).
- **Expanded Remote Codes:** In addition to the 61 universal POWER codes, added verified MUTE, Volume Up/Down, Channel Up/Down, and Source Input codes across NEC, Samsung, Sony SIRC, and Philips RC5/RC6.
- **Action & Alias Matching:** `universal_ir_matches_action()` allows queries like `"Power"`, `"Power Off"`, `"pwr"`, `"TURNHISTVOFF"`, `"vol+"`, `"mute"` to trigger the correct signal set.

---

## 3. SD Card Manager Audit & Optimizations

### 3.1 SPI vs. SDMMC Mode Configuration
The firmware supports four distinct storage backends:
1. **SDMMC 1-Bit Mode (`CONFIG_USING_MMC_1_BIT`):** Uses SDMMC host controller (Slot 0/1) with CLK, CMD, and D0. Ideal for boards with limited GPIOs (e.g., CrowPanel P4, custom S3 hardware).
2. **SDMMC 4-Bit Mode (`CONFIG_USING_MMC`):** High-speed 4-bit bus (CLK, CMD, D0–D3) for high-throughput packet captures and asset streaming.
3. **SPI Mode (`CONFIG_USING_SPI`):** Standard SD SPI bus (CS, CLK, MISO, MOSI). Supports shared display multiplexing.
4. **Virtual Flash Storage:** On-board flash partition (`storage`, FATFS with wear leveling) for devices lacking physical micro-SD slots (e.g., M5Stack S3 T-Watch, AtomS3R).

### 3.2 Pin Decoupling & Board Configuration Linking
- **Elimination of Hardcoded Constants:** In `sd_card_manager.c`, the default structure now sources pins directly from central Kconfig defines (`CONFIG_SD_MMC_CLK`, `CONFIG_SD_MMC_CMD`, `CONFIG_SD_MMC_D0`, `CONFIG_SD_SPI_CS_PIN`, etc.), falling back to `-1` (unassigned) rather than assuming arbitrary GPIOs (19, 18, 20...).
- **Honoring Runtime Configuration:** Corrected `sd_card_init()` in 1-bit mode to use `sd_card_manager.clkpin`, `cmdpin`, and `d0pin` when configured via NVS or `sd_pins_mmc` instead of ignoring them.
- **Pin Range Fix:** Extended pin validation in `cmd_sd.c` from `> 40` to `> 48` to support ESP32-S3 high GPIOs (GPIO 41–48).
- **Corrected Pull-up Configuration:** Fixed inverted pull-up assignments in MMC 1-bit initialization.

### 3.3 Robust Error Handling & Lifecycle State Tracking
A formal status tracking engine has been introduced to prevent blind mounting attempts and boot loops:
```c
typedef enum {
    SD_STATUS_NOT_INITIALIZED = 0,
    SD_STATUS_MOUNTED,
    SD_STATUS_NO_CARD,
    SD_STATUS_UNFORMATTED,
    SD_STATUS_BUS_ERROR,
    SD_STATUS_UNMOUNTED
} sd_card_status_t;
```
- **Error Disambiguation:**
  - `ESP_ERR_TIMEOUT` -> `SD_STATUS_NO_CARD` ("No card detected").
  - `ESP_FAIL` -> `SD_STATUS_UNFORMATTED` ("Unformatted/corrupt filesystem").
  - Other bus faults -> `SD_STATUS_BUS_ERROR` ("Hardware/bus error").
- **Graceful Boot Behavior:** The deferred SD mount task (`main.c`) logs a single descriptive warning and halts further storage operations gracefully without panic or reboot cycling.
- **Dynamic CLI Mount/Unmount:** Users can now run `sd mount` and `sd unmount` from the serial terminal to hot-swap cards without resetting the microcontroller.

### 3.4 Shared Display SPI Bus Arbitration & JIT Mount
For CYD (Cheap Yellow Display) and C5 boards where the display and SD card share the SPI2/SPI3 bus:
- **JIT Mounting (`sd_card_jit_begin` / `sd_card_jit_end`):** Unmounts the card FatFS VFS immediately after flush operations to release the SPI bus back to LVGL.
- **Transaction Draining:** Before SD claims the bus, pending display DMA descriptors are drained (`disp_wait_for_pending_transactions()`) and the display CS pin is pulled HIGH, preventing pixel tearing or display controller lockups.

---

## 4. Peripheral Health & Life Cycle Management

### 4.1 SubGHz / CC1101 Transceiver
- **Presence Verification:** `subghz_remote_manager.c` issues `CC1101_STROBE_SRES` followed by reading status registers `CC1101_STATUS_VERSION` and `CC1101_STATUS_PARTNUM`.
- **Validation:** If both version and partnum read `0x00` or `0xFF`, the transceiver is flagged as disconnected (`cc1101 not detected`) and the SPI bus is safely released.
- **State Reporting:** `subghz_remote_manager_is_ready()` provides an immediate boolean query for higher-level managers.

### 4.2 LoRa / SX1262 Transceiver
- **Presence Verification:** `lora_manager.c` probes the SPI interface and queries the SX1262 operating mode and sync word.
- **Safety Ceilings:** Transmit power is capped at hardware limits (22 dBm for SX1262, 17 dBm for SX1276-class devices) to avoid over-current on 3.3V LDO rails.

### 4.3 Infrared Subsystem Self-Test
The new `ir test` CLI command provides an instant diagnostic suite:
1. Validates that configured TX and RX pins are within valid chip GPIO boundaries.
2. Verifies integrity of the Universal IR lookup database.
3. Tests dynamic initialization and deinitialization of the RX ISR and timer subsystem.

---

## 5. Summary of Code Improvements

| File | Changes Made |
|------|-------------|
| `include/core/universal_ir.h` | Removed static duplicate signal array. Added `universal_ir_action_t` enum, extern declarations, action query prototypes, and action counting helpers. |
| `main/core/universal_ir.c` | Implemented single `.rodata` universal signal table (61 original POWER codes + MUTE, VOL+, VOL-, CH+, CH-, INPUT). Implemented action matching and alias resolution. |
| `main/core/commands/cmd_ir.c` | Fixed `"TURNHISTVOFF"` button matching bug. Added `ir raw`, `ir info`, and `ir test` CLI commands. Enhanced `universals list` with action breakdowns. |
| `include/managers/sd_card_manager.h` | Added `sd_card_status_t` lifecycle enum and prototypes: `sd_card_get_status()`, `sd_card_get_status_str()`, `sd_card_is_mounted()`, `sd_card_is_present()`. |
| `main/managers/sd_card_manager.c` | Decoupled hardcoded pins to Kconfig defaults (`-1` fallback). Fixed 1-bit MMC pin assignments and pullups. Added lifecycle state tracking across all mount/unmount paths. Increased RMT TX queue depth and dynamic pin re-target handling. |
| `main/core/commands/cmd_sd.c` | Expanded GPIO pin validation to GPIO 48 (ESP32-S3 support). Added `sd mount` and `sd unmount` commands. Enhanced `sd status` with health and failure diagnostic output. |
| `main/managers/infrared_manager.c` | **Hardware Safety:** Removed continuous DC HIGH pin drive (`gpio_set_level(..., 1)`), enforcing safe idle-low state. Enhanced RMT TX channel allocation with dynamic pin tracking and increased queue depth. |

---

## 6. Verification & Recommendations

1. **Compilation & Syntax:** All header and source files use strict C99/C11 standards compliant with ESP-IDF v5.x and v6.x, without compiler warnings or symbol collisions.
2. **Backwards Compatibility:** Legacy callers referencing `UNIVERSAL_IR_SIGNAL_COUNT` or `sd_card_manager.is_initialized` remain fully compatible.
3. **Hardware Recommendation:** Ensure external IR blasters utilize an N-channel MOSFET (e.g., 2N7002, BSS138, or IRLML2502) with a gate pull-down resistor (10 kΩ) to ground, preventing floating gate turn-on during ESP32 bootloader execution.
