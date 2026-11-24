# System Requirements Specification (SRS)
## GhostESP God Mode V2 – Master Re-Engineering, Architecture & Optimization Directive

**Document Version:** 2.0.0  
**Status:** Approved / Active Baseline  
**Date:** November 24, 2026  
**Target Platform:** 7Semi ESP32-S3-WROOM-1-N16R8 (Dual USB-C, 16 MB Flash, 8 MB Octal PSRAM)  
**Author:** Principal Embedded Systems Architect & Autonomous Engineering Organization  

---

## 1. Executive Summary & Objective

The objective of **GhostESP God Mode V2** is to elevate the existing GhostESP firmware into an enterprise-grade, highly modular, deterministic, and resilient wireless research and hardware diagnostic platform. 

GhostESP God Mode V2 enforces:
1. **Absolute Hardware Pin Authority:** Direct, non-conflicting mapping against the official 7Semi ESP32-S3 Devkit pinout reference.
2. **Unified Hardware Abstraction Layer (HAL):** Dynamic peripheral registration, conflict detection, and graceful degradation when modules are physically disconnected.
3. **Optimized Display & Compact UI Engine:** Native ST7735S 128×160 SPI TFT integration with intelligent typography scaling, zero-overflow view layouts, and responsive controls.
4. **Wireless & RF Coexistence Stability:** Hardened Wi-Fi promiscuous analysis, BLE scanner stabilization, and dedicated NRF24L01+ 2.4 GHz spectrum sweep engine on a dedicated SPI bus without bus contention.
5. **Memory & FreeRTOS Determinism:** Rigorous stack sizing, PSRAM allocation for buffers/PCAPs, deterministic task priorities, and watchdog timer starvation prevention.

---

## 2. Hardware Architecture & Target Specification

### 2.1 Target Microcontroller Subsystem
- **Core:** Dual-Core Xtensa® 32-bit LX7 CPU @ up to 240 MHz + Low-Power (LP) Core.
- **Embedded Flash:** 16 MB SPI Flash configured in Dual I/O (DIO) mode at 80 MHz.
- **External PSRAM:** 8 MB Octal SPI PSRAM (`AP_3v3`) operating at 80 MHz DDR.
- **Power Delivery:** 5V supplied via Primary USB-C (Native) or Secondary USB-C (UART), regulated via onboard LDOs to 3.3V logic.

### 2.2 Primary Hardware Pinout Specification (7Semi Reference)

| Subsystem / Peripheral | Signal | GPIO Pin | Hardware Direction | Electrical / Functional Constraints |
| :--- | :--- | :---: | :---: | :--- |
| **Primary I2C Bus** | `SDA`<br>`SCL` | **GPIO8**<br>**GPIO9** | Bi-dir (Open-Drain)<br>Output (Open-Drain) | External 4.7kΩ pull-ups to 3.3V required. Used for I2C sensors, RTC, OLED expansions. |
| **External RF SPI (SPI2)** | `CS`<br>`MOSI`<br>`SCK`<br>`MISO`<br>`CE` | **GPIO10**<br>**GPIO11**<br>**GPIO12**<br>**GPIO13**<br>**GPIO14** | Output<br>Output<br>Output<br>Input<br>Output | Dedicated exclusively to **NRF24L01+PA+LNA** transceiver module. Driven at 8 MHz SPI clock. Socket adapter VCC connected to 5V (VIN) for onboard AMS1117 regulator. |
| **Physical Display (SPI3)**| `MOSI`<br>`SCK`<br>`CS`<br>`DC`<br>`RST`<br>`BCKL` | **GPIO15**<br>**GPIO16**<br>**GPIO5**<br>**GPIO6**<br>**GPIO7**<br>**3.3V (NC)** | Output<br>Output<br>Output<br>Output<br>Output<br>Power Rail | Dedicated to **1.8" ST7735S 128×160 SPI TFT** display. Driven on SPI3 (`SPI3_HOST`) to eliminate bus sharing contention with NRF24. |
| **UART1 Interface** | `TX1`<br>`RX1` | **GPIO43**<br>**GPIO44** | Output<br>Input | High-speed auxiliary serial / GPS / external modem telemetry. |
| **UART2 Interface** | `TX2`<br>`RX2` | **GPIO17**<br>**GPIO18** | Output<br>Input | Secondary serial bridge / expansion interface. |
| **Native USB Subsystem** | `D-`<br>`D+` | **GPIO19**<br>**GPIO20** | Bi-directional | **RESTRICTED:** Dedicated exclusively to native USB-JTAG/CDC and BadUSB HID injection. Never allocate for general GPIO! |
| **Onboard RGB Indicator**| `DATA` | **GPIO48** | Output | Built-in WS2812B NeoPixel RGB LED driven via high-precision RMT peripheral (GRB 800 kHz). |
| **Real-Time Clock / Aux** | `RTC_INT` | **GPIO21** | Input | Auxiliary RTC interrupt / interrupt line. |
| **Strapping / Forbidden** | `BOOT`<br>`STRAP`<br>`OCTAL PSRAM`<br>`JTAG` | **GPIO0**<br>**GPIO3, 45, 46**<br>**GPIO35–38**<br>**GPIO39–42** | Special | **GPIO46 is INPUT-ONLY.** GPIO35–38 are hardwired to internal 8MB Octal PSRAM bus; reassigning them causes immediate hardware crash. |

---

## 3. Functional Requirements (FR)

### Module 1: Hardware Abstraction & Pin Registry (HAR)
- **FR-HAR-01:** The firmware shall maintain a single source of truth for all GPIO assignments (`include/hardware/board_config.h`).
- **FR-HAR-02:** A dynamic Pin Registry (`main/hardware/board_config.c`) shall track peripheral registration, pin direction, lock states, and shared bus ownership.
- **FR-HAR-03:** Any attempt to allocate restricted GPIOs (GPIO19/20 USB, GPIO35–38 Octal PSRAM, GPIO0/45/46 strapping violations) must fail gracefully with explicit runtime diagnostics.
- **FR-HAR-04:** Every peripheral driver must perform a hardware presence check during initialization and degrade gracefully if the hardware is disconnected without crashing or panic loops.

### Module 2: Wireless Subsystem (WIR)
- **FR-WIR-01:** The Wi-Fi subsystem shall support 2.4 GHz 802.11 b/g/n active scanning, passive promiscuous frame sniffing, and raw packet parsing.
- **FR-WIR-02:** Promiscuous packet reception buffers shall be dynamically allocated in external PSRAM when available to prevent SRAM exhaustion.
- **FR-WIR-03:** Wireless operations shall enforce strict ethical security research boundaries: no credential extraction, no automated unauthorized access, and no unconstrained network denial-of-service.
- **FR-WIR-04:** Channel switching and hopping routines shall utilize non-blocking FreeRTOS timers with configurable dwell times (50 ms – 1000 ms).

### Module 3: Bluetooth & BLE Engine (BLE)
- **FR-BLE-01:** The BLE engine shall support active/passive GAP scanning for BLE advertisements, iBeacons, FindMy trackers, and GATT services.
- **FR-BLE-02:** BLE scan results shall be cached in a ring buffer with LRU eviction and vendor OUI lookup.
- **FR-BLE-03:** BLE and Wi-Fi coexistence shall prevent radio arbiter lockups and FreeRTOS task starvation.

### Module 4: NRF24L01+ 2.4 GHz RF Analyzer (NRF)
- **FR-NRF-01:** The NRF24 driver shall operate on `SPI2_HOST` at 8 MHz with hardware CSN (GPIO10) and CE (GPIO14).
- **FR-NRF-02:** The spectrum analyzer sweep shall monitor 126 channels (2.400 GHz – 2.525 GHz) with configurable dwell times.
- **FR-NRF-03:** The receiver sampling loop shall maintain CE asserted across the dwell window to allow the Receive Power Detector (RPD) to latch reliably.
- **FR-NRF-04:** Signal hit strength shall be mapped into a visual spectrum chart and waterfall plot with configurable decay smoothing.

### Module 5: Infrared (IR) Framework (IRF)
- **FR-IRF-01:** The IR subsystem shall utilize the ESP-IDF v6 RMT peripheral for pulse-width modulation and pulse capture.
- **FR-IRF-02:** Supported decoders: NEC, Samsung, Sony (SIRC), RC5, and raw pulse-distance/mark timings.
- **FR-IRF-03:** IR output drivers must enforce hardware safety: GPIO pin drive currents must not exceed 20 mA; external N-channel MOSFET or NPN transistor circuits are mandatory for high-power emitters.

### Module 6: User Interface & Display (UID)
- **FR-UID-01:** Display driver: ST7735S 128×160 RGB SPI TFT over `SPI3_HOST`.
- **FR-UID-02:** All LVGL UI screens shall adhere to compact screen tokens (`GUI_GRID=2`, `GUI_STATUS_BAR_H=22`, `GUI_CONTROL_H=24`).
- **FR-UID-03:** Dynamic font selection shall prioritize Montserrat 8 pt and 10 pt to prevent text truncation on 128 px width panels.
- **FR-UID-04:** Interactive views (Options view, NRF24 analyzer, Airspace monitor) must implement responsive layouts with zero off-screen rendering.

### Module 7: BadUSB & USB Peripheral Emulation (USB)
- **FR-USB-01:** TinyUSB HID composite device stack shall support 3 endpoints: Keyboard, Mouse, and Gamepad.
- **FR-USB-02:** Script executor (`DuckyScript` parser) shall support standard syntax, modifier keys, delays, and strings.

---

## 4. Non-Functional Requirements (NFR)

- **NFR-PERF-01 (CPU Utilization):** Idle CPU load must remain below 10% on Core 0 and Core 1.
- **NFR-PERF-02 (Memory Headroom):** Minimum free internal SRAM watermark must remain above 64 KB at all times during heavy scanning.
- **NFR-PERF-03 (Display Refresh):** UI frame rate on the 128×160 panel shall maintain 25–30 FPS without tearing.
- **NFR-REL-01 (Crash Resilience):** Zero uncaught exceptions or null-pointer panics (`LoadProhibited`). All peripheral API calls must return error codes.
- **NFR-SEC-01 (Safe Memory Management):** Bounds checking on all serial commands, packet parsers, and file operations.

---

## 5. Verification Matrix

| Requirement ID | Verification Method | Acceptance Criteria |
| :--- | :--- | :--- |
| **FR-HAR-01/02** | Unit & Static Analysis | Centralized board config compiles and tracks all 7Semi pins without conflicts. |
| **FR-WIR-01/02** | Hardware Integration | Wi-Fi scan discovers 2.4 GHz APs, parses beacons, and logs signal levels. |
| **FR-NRF-01/02** | Hardware Integration | NRF24 spectrum analyzer executes 126-channel sweep on SPI2; graph draws on display. |
| **FR-UID-01/02** | Visual Inspection | ST7735S display renders boot screen, status bar, and menus with zero clipped elements. |
| **NFR-REL-01** | Stress Testing | 1-hour continuous Wi-Fi + NRF24 + UI execution without memory leak or reboot. |
