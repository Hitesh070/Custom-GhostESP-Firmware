# GhostESP God Mode V3 – Master Feature Gap Analysis

**Version:** 3.0.0  
**Target Hardware:** 7Semi ESP32-S3-WROOM-1-N16R8  
**Engineering Authority:** Lead Systems Architect & Project Management  
**Date:** November 24, 2026  

---

## 1. Feature Status Classification Matrix

Every feature specified in the God Mode V3 directive is evaluated against the existing codebase (post-V2 re-engineering) to classify its current maturity and map out technical work required.

| Master Category | Subsystem / Capability | V2 Baseline State | V3 God Mode Gap / Action Required | Maturity Target |
|---|---|---|---|---|
| **Wi-Fi Security** | Advanced Wi-Fi Scanner | Implemented in `wifi_manager.c` | Add vendor OUI lookup, security grade rating, signal history tracking | **Enhanced** |
| **Wi-Fi Security** | Rogue AP / Evil Twin Engine | Basic candidate tracking in `airspace_monitor.c` | Implement standalone multi-vector comparison engine (BSSID, OUI, cipher downgrade, RSSI cliff) | **New Subsystem** |
| **Wi-Fi Security** | Deauth / Disassociation Monitor | Passive probe in `callbacks.c` | Implement event counter, source/target tracker, burst threshold alerts, CLI command | **New Subsystem** |
| **Wi-Fi Security** | Passive Packet Capture | Basic PCAP dump | Add capture filters, live buffer metadata, SD ringbuffer export | **Enhanced** |
| **BLE Research** | BLE Scanner | Implemented in `ble_manager.c` | Add manufacturer data decoding, advertising metadata history | **Enhanced** |
| **BLE Research** | BLE Radar Visualization | Missing / Text list only | Implement polar coordinate radar screen with RSSI-scaled visual nodes | **New Feature** |
| **BLE Research** | GATT Explorer | Basic scan in `gatt_scan.c` | Structured interactive service/characteristic tree with safe read/descriptor inspect | **Enhanced** |
| **NRF24 RF** | NRF24 Hardware Diagnostics | Basic presence check in `board_config.c` | Add comprehensive register bitfield dump (`CONFIG`, `EN_AA`, `STATUS`, `FIFO_STATUS`) | **New Subsystem** |
| **NRF24 RF** | Channel Activity Scanner | Basic 0–127 RSSI/RPD in `nrf24_remote_manager.c` | Add multi-channel occupancy histogram, waterfall view, dwell statistics | **Enhanced** |
| **NRF24 RF** | Link Quality Benchmarking | Missing | Implement controlled packet delivery ratio (PDR), latency, and throughput tester | **New Feature** |
| **Universal IR** | Universal Protocol Database | Expanded in V2 (`universal_ir.c`) | Add raw pulse capture, timing analyzer, protocol classifier (NEC, Sony, RC5/6, Samsung) | **Enhanced** |
| **Universal IR** | Waveform Visualizer | Missing | ASCII / TFT timing trace display of mark/space pulse durations | **New Feature** |
| **USB Lab** | Benign HID Testing | Basic TinyUSB keyboard | Implement safe, visible macro tests with explicit user confirmation and safety guards | **Enhanced** |
| **Architecture** | Central Hardware Authority | Implemented in V2 (`board_config.c`) | Expand with auto-detection probes, register arbitration, and dynamic peripheral state | **Enhanced** |
| **Architecture** | Configuration Profiles | Static NVS settings | Implement Profile Manager: Wi-Fi Lab, BLE Lab, NRF24 Lab, IR Lab, USB Lab, Diagnostics | **New Subsystem** |
| **Architecture** | Firmware Self-Test Suite | Missing | Implement structured automated self-test report covering buses, RAM, PSRAM, and transceivers | **New Subsystem** |
| **Embedded UI** | Ghost Mode & Matrix Mode | Missing | Implement cyberpunk matrix terminal rain, dark ghost visual theme, glitch effects | **New Easter Egg** |
| **Embedded UI** | Ghost Radar & Companion | Partial Ghostchi pet | Add interactive Ghost Companion with milestone reactions and Radar map | **Enhanced** |
| **Embedded UI** | Secret Developer Console | Basic shell commands | Add secret `ghost` CLI namespace (`ghost help`, `ghost status`, `ghost matrix`, etc.) | **New Subsystem** |
| **Embedded UI** | Achievement System | Missing | Add event-driven achievement engine with persistent NVS milestone unlocks | **New Subsystem** |
| **Desktop App** | Laptop Command Center | Missing / WebUI only | Author standalone Python / PySide6 multi-tab desktop application with serial telemetry | **New Ecosystem** |

---

## 2. Technical Risk & Mitigation Strategy

1. **Internal SRAM & Task Stack Starvation:**
   - *Risk:* Adding complex UI animations (Matrix rain) and desktop telemetry tasks could exhaust internal DRAM.
   - *Mitigation:* Allocate heavy framebuffers and packet histories in 8 MB Octal PSRAM (`MALLOC_CAP_SPIRAM`). Keep FreeRTOS task stacks strictly tuned.
2. **SPI Bus Collisions:**
   - *Risk:* NRF24 and Display traffic interfering.
   - *Mitigation:* Preserved the God Mode V2 dual-SPI partition: `SPI2_HOST` strictly for NRF24 (8 MHz) and `SPI3_HOST` strictly for ST7735S (20 MHz).
3. **Safety & Active Transmission Guardrails:**
   - *Risk:* Inadvertent RF interference.
   - *Mitigation:* Passive observation is default; all active tests require explicit user confirmation and operate only in authorized lab profiles.
