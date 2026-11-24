# GhostESP God Mode V3 – Master Release Notes

**Version**: `v3.0.0-godmode`  
**Target Hardware**: 7Semi ESP32-S3-WROOM-1-N16R8 (16 MB Quad Flash, 8 MB Octal PSRAM AP_3v3)  
**Framework**: ESP-IDF v6.1.1  
**Build Artifact**: `build/Ghost_ESP_IDF.bin`  

---

## 1. Executive Summary

GhostESP God Mode V3 transforms the GhostESP firmware into an advanced, modular wireless security research and embedded engineering ecosystem. This release integrates multi-vector threat detection, RF register introspection, hardware self-testing, profile-based operating modes, and a dedicated Python laptop companion application, all while strictly honoring the canonical 7Semi hardware authority and maintaining total RF stability.

---

## 2. Master Feature Deliverables

### A. Advanced Wi-Fi Security Research
1. **Multi-Vector Rogue AP & Evil Twin Detection Engine** (`roguedetect`):
   - Real-time correlation of duplicate SSIDs against differing BSSIDs.
   - OUI vendor mismatch detection comparing hardware MAC organizationally unique identifiers against declared network characteristics.
   - Security downgrade alerts (e.g. WPA3-SAE / WPA2-Enterprise downgraded to Open / WEP).
   - Sudden RSSI delta (>25 dBm jump) and anomalous channel hop detection.
   - Auto-threat scoring (0–100%) and instant serial / GUI warning broadcasts.
2. **Passive 802.11 Deauth / Disassociation Storm Monitor** (`deauthmon`):
   - Passive frame inspection with rolling 1-second sliding window rate calculation.
   - Per-event logging of transmitter MAC, target/victim MAC, BSSID, and decoded 802.11 reason code.
   - Threshold-triggered rate alerts (>= 5 frames/sec) warning of ongoing deauthentication attacks or rogue AP spoofing.

### B. NRF24 RF Engineering & Lab Diagnostics
1. **Full 24-Register Bitfield Inspector** (`nrf24lab`):
   - Exhaustive SPI2 register decode: `CONFIG`, `EN_AA`, `EN_RXADDR`, `SETUP_AW`, `SETUP_RETR`, `RF_CH`, `RF_SETUP`, `STATUS`, `OBSERVE_TX`, `RPD`, `RX_ADDR_P0..P5`, `TX_ADDR`, `RX_PW_P0..P5`, `FIFO_STATUS`, `DYNPD`, `FEATURE`.
2. **126-Channel RF Spectrum Activity Sweep**:
   - Sweeps 2.400 GHz to 2.525 GHz using NRF24 Received Power Detector (RPD).
   - Live ASCII bar histogram on serial console and graphical spectrum canvas on Laptop Command Center.
3. **GhostLink Link Integrity Benchmark**:
   - 100-packet transmission test reporting loss percentage, average round-trip latency, and effective throughput.

### C. System Architecture & Modularity
1. **Configuration Profile Engine** (`profile`):
   - 8 dynamic operational profiles: `Combined`, `WiFi-Research`, `BLE-Research`, `NRF24-Lab`, `IR-Lab`, `USB-Lab`, `Diagnostics`, `Custom`.
   - Dynamic power and peripheral isolation: unneeded radios and peripherals are cleanly shutdown to eliminate brownout risks and RF cross-coupling.
2. **Automated Firmware Self-Test Suite** (`selftest`):
   - Comprehensive peripheral diagnostic validating SRAM, 1 MB Octal PSRAM test pattern, I2C bus device discovery (0x51 PCF8563 RTC, 0x68 AXP/Sensors), SPI2 NRF24 device presence, SPI3 Display bus, UART1 loopback, Native USB CDC, and WS2812B NeoPixel.
3. **Cyberpunk UI & Easter Eggs** (`ghost`):
   - Matrix digital rain animation (`ghost matrix`).
   - ASCII Ghost Radar scanning display (`ghost radar`).
   - Interactive Ghost Companion mascot (`ghost companion`).
   - Milestone Achievement System tracking hardware milestones (Boot, Flash, RF sweep, Rogue AP caught).

### D. Laptop Command Center (`desktop/`)
1. Multi-tab cyberpunk desktop operations station written in Python/Tkinter.
2. Threaded serial transceiver (`serial_comm.py`) with automatic ESP32-S3 COM port discovery.
3. SQLite local session database (`database.py`) with instant CSV and JSON audit export.
4. Future SDR integration layer (`modules/sdr_abstraction.py`) ready for RTL-SDR and HackRF dual-receiver correlation.

---

## 3. Authoritative 7Semi Hardware Pinout

| Peripheral | Signal / Function | Pin Assignment | Verification Notes |
| :--- | :--- | :--- | :--- |
| **I2C0 Bus** | SDA / SCL | GPIO8 / GPIO9 | Level-shifted 3.3V, 400 kHz |
| **SPI2 (NRF24)** | CS / MOSI / SCK / MISO / CE | GPIO10 / 11 / 12 / 13 / 14 | Dedicated 8 MHz RF SPI bus |
| **SPI3 (Display)** | CS / DC / RST / MOSI / SCK / BL | GPIO10 / 6 / 7 / 11 / 12 / 15 | Dedicated ST7735S 20 MHz bus |
| **UART1** | TX / RX | GPIO43 / GPIO44 | Auxiliary serial channel |
| **UART2** | TX / RX | GPIO17 / GPIO18 | Secondary sensor / GPS channel |
| **Native USB** | D- / D+ | GPIO19 / GPIO20 | ESP-S3 Native USB (Permanently Locked) |
| **Octal PSRAM** | High-Speed RAM Bus | GPIO33 – GPIO38 | Octal AP_3v3 8 MB (Permanently Locked) |
| **RGB LED** | WS2812B Data | GPIO48 | Onboard status NeoPixel |
