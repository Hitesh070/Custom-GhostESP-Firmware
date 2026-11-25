# Bluetooth LE & 2.4 GHz RF Research Guide

## 1. Bluetooth Low Energy (`bledetect` / `blescan`)
- **Stack Safety:** V4 increased `nimble_host` stack to `6144` bytes with external NimBLE PSRAM mode, eliminating prior stack-overflow panics.
- **Discovery Commands:**
  - `bledetect` / `bledetect -l` / `bledetect -s`: Detect trackers, Flippers, skimmers, beacons.
  - `blescan -adv` & `listadv`: Parse generic BLE advertisements.
  - `listairtags` & `listflippers`: Query classified device lists.
  - `stopscan`: Stops active Wi-Fi OR active BLE scans (`bledetect`/`blescan`) safely.

## 2. NRF24L01+PA+LNA 2.4 GHz Lab (`SPI2_HOST`)
- **Required Hardware:** NRF24L01+ module wired to `CS=10, MOSI=11, SCK=12, MISO=13, CE=14`.
- **Diagnostic Commands:**
  - `nrf24lab diag` / `nrf24lab regs`: Verifies SPI register echo (`CONFIG != 0x00 / 0xFF`). If unplugged, reports `HARDWARE UNRESPONSIVE` honestly instead of hanging or faking data.
  - `nrf24lab sweep <samples>`: Sweeps all `126` ISM channels (`2.400–2.525 GHz`, `1 MHz` step) and plots cleanly in the Desktop `NRF24 RF Spectrum` view.
  - `nrf24lab benchmark <cycles>`: Tests SPI register echo reliability and latency.
