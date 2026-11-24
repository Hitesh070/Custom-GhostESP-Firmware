# GhostESP God Mode V3 – System Resource & Concurrency Registry

**Hardware Target:** 7Semi ESP32-S3-WROOM-1-N16R8 (240 MHz Dual-Core LX7)  
**Memory Architecture:** 512 KB Internal SRAM + 8 MB Octal PSRAM (`AP_3v3` @ 40 MHz DDR)  
**Storage Architecture:** 16 MB SPI Flash (`DIO` mode @ 80 MHz)  

---

## 1. Hardware Bus & Silicon Concurrency Matrix

| Bus / Peripheral | Host Controller | Assigned GPIOs | Clock Frequency | Dedicated Subsystem | Concurrency Policy |
|---|---|---:|---|---|---|
| **I2C Bus 0** | `I2C_NUM_0` | SDA: 8, SCL: 9 | 400 kHz | Sensors, External RTC | Shared, Mutex-Protected |
| **RF SPI Bus** | `SPI2_HOST` (FSPI) | CS: 10, MOSI: 11, SCK: 12, MISO: 13, CE: 14 | 8 MHz | NRF24L01+PA+LNA | **Strictly Dedicated** |
| **Display SPI Bus** | `SPI3_HOST` (HSPI) | CS: 5, DC: 6, RST: 7, MOSI: 15, SCK: 16 | 20 MHz | ST7735S 128×160 TFT | **Strictly Dedicated** |
| **Serial UART 1** | `UART_NUM_1` | TX: 43, RX: 44 | 115200 Baud | High-Speed Telemetry / GPS | Dedicated Stream |
| **Serial UART 2** | `UART_NUM_2` | TX: 17, RX: 18 | 115200 Baud | Auxiliary Expansion | Dedicated Stream |
| **Native USB** | `USB_OTG_FS` | DM: 19, DP: 20 | 12 Mbps (FS) | TinyUSB CDC / BadUSB HID | **System Reserved** |
| **RGB Indicator** | RMT / LEDC | Data: 48 | 800 kHz | WS2812B NeoPixel | Shared Driver |
| **Octal PSRAM** | High-Speed MSPI | 33, 34, 35, 36, 37, 38 | 40 MHz DDR | 8 MB External Heap | **Permanently Locked** |
| **SPI Flash** | Internal MSPI | 27, 28, 29, 30, 31, 32 | 80 MHz | 16 MB Code / Storage | **Permanently Locked** |

---

## 2. FreeRTOS Task Affinity & Memory Budget

| Task Name | Core Affinity | Priority | Stack Size (Bytes) | RAM Target | Subsystem Owner |
|---|---|---|---|---|---|
| `main_app` | Core 0 | 1 | 8,192 | Internal SRAM | Firmware Bootstrap |
| `display_task` | Core 1 | 5 | 10,240 | Internal SRAM + PSRAM | LVGL 8.3 GUI Pipeline |
| `wifi_rx_task` | Core 0 | 6 (High) | 6,144 | Internal SRAM | Promiscuous RX Buffer & Detection |
| `ble_host_task` | Core 0 | 4 | 4,096 | Internal SRAM | NimBLE Bluetooth Stack |
| `nrf24_sweep` | Core 1 | 3 | 4,096 | Internal SRAM | 2.4 GHz RF Analyzer Sweep |
| `sd_io_task` | Core 0 | 2 | 8,192 | Internal SRAM | Async FAT32 File Operations |
| `telemetry_tx` | Core 1 | 2 | 4,096 | Internal SRAM | Laptop Command Center Serial Sync |
| `matrix_anim` | Core 1 | 1 (Idle+1) | 4,096 | Octal PSRAM | Matrix Mode Cyberpunk Renderer |

---

## 3. Dynamic Heap Partitioning

- **Internal SRAM Heap (512 KB total, ~320 KB free at boot):**
  - Reserved for FreeRTOS task stacks, ISR vectors, DMA-capable SPI/I2C descriptors, and Wi-Fi MAC buffers.
  - Hard constraint: `CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL=32768` guarantees 32 KB minimum internal memory available for ISRs at all times.
- **External Octal PSRAM Heap (8,192 KB total):**
  - Reserved for LVGL double-framebuffers, packet history rings, universal IR pulse arrays, and desktop telemetry serialization buffers.
  - Allocated dynamically with `heap_caps_malloc(size, MALLOC_CAP_SPIRAM)`.
