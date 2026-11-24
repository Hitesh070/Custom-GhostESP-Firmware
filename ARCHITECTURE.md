# Master Architecture Specification
## GhostESP God Mode V2 – High-Reliability Embedded Firmware Architecture

**Document Version:** 2.0.0  
**Target Architecture:** Espressif ESP32-S3 (Xtensa Dual-Core LX7 @ 240 MHz, 16 MB Flash, 8 MB Octal PSRAM)  
**Status:** Approved Architecture Baseline  
**Date:** November 24, 2026  

---

## 1. Architectural Overview & Design Philosophy

GhostESP God Mode V2 is engineered around four core tenets:
1. **Decoupled Hardware Abstraction:** No application manager or UI screen touches raw GPIO registers directly. All peripherals access pins via a centralized Board Configuration & Pin Allocation Registry.
2. **Dedicated Bus Separation:** High-frequency SPI traffic (RF spectrum scanning) and visual SPI traffic (display frame rendering) reside on independent physical SPI controllers (`SPI2_HOST` and `SPI3_HOST`), eliminating bus contention, mutex jitter, and visual stutter.
3. **Determinism & Memory Hierarchy:** Heap-heavy buffers (PCAP capture pools, BLE discovery tables, font glyph caches) are allocated dynamically in external 8 MB Octal PSRAM, preserving precious internal SRAM for high-speed FreeRTOS task stacks and ISR buffers.
4. **Resilient Failure Handling:** Missing or unpowered hardware modules degrade gracefully with warning logs instead of inducing kernel panics (`LoadProhibited` / `Guru Meditation`).

---

## 2. System Architecture Block Diagram

```mermaid
graph TB
    subgraph Hardware ["Physical Hardware Layer (7Semi ESP32-S3 Devkit)"]
        MCU["ESP32-S3 Dual-Core LX7 @ 240MHz<br>(16MB Flash | 8MB Octal PSRAM)"]
        NRF["NRF24L01+PA+LNA<br>(2.4GHz RF Transceiver)"]
        TFT["1.8' ST7735S TFT<br>(128x160 RGB Display)"]
        RGB["WS2812B NeoPixel<br>(Onboard RGB LED)"]
        USB["Dual USB-C Ports<br>(Native USB & UART)"]
        I2CBus["I2C External Bus<br>(SDA: 8, SCL: 9)"]
        UART1Bus["UART1 Header<br>(TX: 43, RX: 44)"]
        UART2Bus["UART2 Header<br>(TX: 17, RX: 18)"]
    end

    subgraph BusPartition ["Bus & Peripheral Partitioning"]
        SPI2["SPI2_HOST (FSPI)<br>MOSI:11, MISO:13, SCK:12, CS:10, CE:14"]
        SPI3["SPI3_HOST (SPI3)<br>MOSI:15, SCK:16, CS:5, DC:6, RST:7"]
        RMT["RMT Peripheral<br>GPIO48 (800 kHz GRB)"]
        USB_IP["USB-OTG Peripheral<br>GPIO19 (D-), GPIO20 (D+)"]
        I2C_IP["I2C0 Peripheral<br>GPIO8 (SDA), GPIO9 (SCL)"]
    end

    subgraph HAL ["Hardware Abstraction Layer (HAL)"]
        BCFG["Board Config Authority<br>(board_config.h)"]
        PREG["Pin Allocation Registry<br>(board_config.c)"]
        PWR["Power & Rail Monitor"]
    end

    subgraph CoreManagers ["Subsystem & Manager Layer"]
        WIFI_MGR["Wi-Fi Manager<br>(Promiscuous Sniffing / Scans)"]
        BLE_MGR["BLE Manager<br>(GAP Discovery / Trackers)"]
        NRF_MGR["NRF24 Remote Manager<br>(126-Ch Spectrum Analyzer)"]
        RGB_MGR["RGB Manager<br>(Knight, Police, Strobe, Rainbow)"]
        USB_MGR["TinyUSB HID Manager<br>(BadUSB Keyboard / Mouse)"]
        SD_MGR["SD Card Manager<br>(VFS / PCAP Logger)"]
        IR_MGR["Universal IR Manager<br>(RMT TX / RX Decoders)"]
        SET_MGR["Settings Manager<br>(NVS Flash Configuration)"]
    end

    subgraph Presentation ["Presentation & UI Layer"]
        LVGL["LVGL v8 / v9 Graphics Engine"]
        TOKENS["Compact Design Tokens<br>(Grid=2, Status=22, Control=24)"]
        VIEWS["Modular Views<br>(Options, NRF24, Airspace, Settings)"]
        CLI["Serial Command Engine<br>(Dual UART & WebSerial CLI)"]
    end

    %% Wiring connections
    MCU --> SPI2 & SPI3 & RMT & USB_IP & I2C_IP
    SPI2 --> NRF
    SPI3 --> TFT
    RMT --> RGB
    USB_IP --> USB
    I2C_IP --> I2CBus

    %% Abstraction links
    SPI2 & SPI3 & RMT & USB_IP & I2C_IP --> BCFG & PREG
    BCFG & PREG --> CoreManagers
    CoreManagers --> Presentation
```

---

## 3. Bus Architecture & Pin Allocation Table

GhostESP God Mode V2 maintains strict bus isolation across the 7Semi ESP32-S3 board:

| Subsystem | Bus / Host | MOSI / SDA / TX | MISO / SCL / RX | Clock / SCK | Chip Select (CS) | Extra Control |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **NRF24 RF Transceiver** | `SPI2_HOST` | GPIO 11 | GPIO 13 | GPIO 12 | GPIO 10 | CE: GPIO 14 |
| **ST7735S 128×160 TFT** | `SPI3_HOST` | GPIO 15 | NC (N/A) | GPIO 16 | GPIO 5 | DC: 6, RST: 7, BCKL: 3.3V |
| **Primary I2C Bus** | `I2C_NUM_0` | GPIO 8 (SDA) | GPIO 9 (SCL) | Up to 400 kHz | N/A | Ext 4.7kΩ Pull-ups |
| **UART1 Auxiliary** | `UART_NUM_1`| GPIO 43 (TX) | GPIO 44 (RX) | N/A | N/A | Up to 921600 baud |
| **UART2 Expansion** | `UART_NUM_2`| GPIO 17 (TX) | GPIO 18 (RX) | N/A | N/A | Up to 115200 baud |
| **Native USB Stack** | `USB-OTG` | GPIO 19 (D-) | GPIO 20 (D+) | N/A | N/A | Reserved for BadUSB |
| **WS2812B RGB Indicator**| `RMT_CH0` | GPIO 48 | N/A | 800 kHz | N/A | Built-in NeoPixel |

### Strict Hardware Restriction Rules:
1. **Octal PSRAM Integrity:** GPIO 35, 36, 37, and 38 connect internally to the 8 MB Octal PSRAM. Never assign, configure, or alter these pins.
2. **Native USB Protection:** GPIO 19 and 20 are routed directly to the native USB-C port. Any general-purpose output toggle on these pins terminates USB communication and brick/reboot the device.
3. **Input-Only Constraint:** GPIO 46 is physically an input-only pin without internal pull-ups or output drivers. It must never be configured as an output.
4. **Boot & Strapping Safety:** GPIO 0 (Boot), GPIO 3, and GPIO 45 are strapping pins. They must not be held in contradictory logic states during power-up or system reset.

---

## 4. FreeRTOS Task Architecture & Concurrency Model

GhostESP God Mode V2 partitions execution across both Xtensa LX7 cores:

```mermaid
sequenceDiagram
    autonumber
    participant Core0 as Core 0 (Protocol & Radio)
    participant Core1 as Core 1 (UI & Application)
    participant Bus2 as SPI2 (NRF24)
    participant Bus3 as SPI3 (Display)

    Note over Core0: Wi-Fi Driver & Promiscuous ISR
    Note over Core1: LVGL Task & GUI Render Loop

    par Radio Sweeping & UI Updating
        Core0->>Bus2: Sweep Channel (Set CH, Assert CE)
        Bus2-->>Core0: Sample RPD latch (Carrier detected)
        Core0->>Core1: Push RF sample to Inter-Core Queue
    and
        Core1->>Bus3: Push LVGL Dirty Rectangles (128x160)
        Bus3-->>Core1: DMA Transfer Complete
    end

    Note over Core1: Redraw Spectrum Plot & Axis
    Core1->>Core1: Update Status Bar & Button States
```

### FreeRTOS Task Allocation Table:

| Task Name | Core Affinity | Priority | Stack Size | Function / Ownership |
| :--- | :---: | :---: | :---: | :--- |
| **`lvgl_task`** | Core 1 | 5 | 8192 bytes | LVGL display rendering, touch/key input polling, view lifecycle. |
| **`serial_cli_task`** | Core 1 | 3 | 4096 bytes | Serial command parser, line editor, and terminal output dispatcher. |
| **`wifi_scan_task`** | Core 0 | 4 | 4096 bytes | 802.11 b/g/n active scanning, beacon parsing, and RSSI tracking. |
| **`nrf24_sweep_task`**| Core 0 | 4 | 4096 bytes | Dedicated 126-channel spectrum sweep and RPD signal acquisition on SPI2. |
| **`rgb_anim_task`** | Core 1 | 2 | 4096 bytes | WS2812B animation engine (Knight Rider, Police, Strobe, Rainbow). |
| **`async_pcap_task`** | Core 0 | 3 | 4096 bytes | Asynchronous disk writing of captured frames to SD Card or SPIFFS. |

---

## 5. Memory Management & Allocation Strategy

```mermaid
graph LR
    subgraph MemorySpace ["ESP32-S3 Physical Memory Space"]
        SRAM["Internal SRAM (512 KB)<br>• FreeRTOS Task Stacks<br>• Wi-Fi / BT DMA Buffers<br>• Critical Interrupt ISRs<br>• Fast Ring Buffers"]
        PSRAM["External Octal PSRAM (8 MB)<br>• LVGL Framebuffers (Double-buffered)<br>• Packet Sniffer PCAP Pool<br>• OUI Database (JSON/Bin table)<br>• BLE Device History Ring Buffer<br>• WebUI Static Assets"]
    end
```

### Memory Rules:
1. **Dynamic PSRAM Allocation:** Large structures ($\ge 4\text{ KB}$) that do not require DMA access must use `heap_caps_malloc(size, MALLOC_CAP_SPIRAM)`.
2. **DMA-Capable Internal Allocations:** SPI transmit buffers and RMT descriptors must be allocated using `heap_caps_malloc(size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL)`.
3. **Internal SRAM Watermark:** The system must maintain at least 64 KB of free internal SRAM at all times to prevent Wi-Fi stack starvation.

---

## 6. Software Layered Architecture

```mermaid
graph TD
    subgraph L5 ["Layer 5: Presentation & User Interface"]
        UI_VIEWS["UI Views (Options, NRF24, Settings, Airspace)"]
        CLI_CMDS["Command Line Handlers (cmd_wifi, cmd_nrf24, cmd_rgb)"]
    end

    subgraph L4 ["Layer 4: Application & Business Logic"]
        SETTINGS["Settings & Persistence Manager (NVS)"]
        ATTACKS["Wireless Assessment & Diagnostic Engines"]
        SCRIPTS["DuckyScript / Macro Interpreters"]
    end

    subgraph L3 ["Layer 3: Core Managers & Subsystems"]
        WIFI["Wi-Fi Subsystem"]
        BLE["BLE Subsystem"]
        NRF["NRF24 Subsystem"]
        RGB_M["RGB Subsystem"]
        DISP["Display Manager"]
        IR["Infrared Subsystem"]
    end

    subgraph L2 ["Layer 2: Hardware Abstraction Layer (HAL)"]
        BOARD["Central Board Config (board_config.h)"]
        PIN_REG["Pin Allocation & Conflict Registry (board_config.c)"]
        DEV_DRV["Peripheral Device Drivers (ST7735S, NRF24, WS2812B)"]
    end

    subgraph L1 ["Layer 1: Operating System & Hardware"]
        IDF["ESP-IDF v6.1 Subsystems"]
        FREERTOS["FreeRTOS SMP Kernel"]
        SILICON["ESP32-S3 Silicon & Breakout Pins"]
    end

    L5 --> L4
    L4 --> L3
    L3 --> L2
    L2 --> L1
```

---

## 7. Error Handling & Architectural Fault Tolerance

1. **Hardware Absence Detection:**
   During boot, every driver attempts a non-destructive handshake (e.g., NRF24 reads `CONFIG` register `0x00`, Display issues software reset `0x01`). If the read returns all 0x00 or all 0xFF, the driver marks the device as `UNAVAILABLE` and continues booting.
2. **Headless & Screen Fallback:**
   If the screen fails to initialize or is unconfigured, GUI calls are short-circuited via `display_manager_is_available()` without attempting to dereference `lv_disp_get_default()`, completely preventing null-pointer crashes.
3. **Timeout Clamping:**
   All inter-task synchronization primitives (mutexes, semaphores, queues) must use bounded timeouts (maximum 1000 ms) instead of `portMAX_DELAY` to prevent deadlock conditions.
