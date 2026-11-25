# SDR Architecture Assessment (Honest Hardware Boundaries)

## 1. Silicon Reality Check (ESP32-S3 vs True SDR)
- The `7Semi ESP32-S3-WROOM-1-N16R8` contains fixed-function **2.4 GHz 802.11 b/g/n Wi-Fi** and **Bluetooth 5.0 LE** baseband radios plus an external SPI header (`SPI2`) for a fixed-GFSK **NRF24L01+PA+LNA** transceiver (`2.400–2.525 GHz`).
- The ESP32-S3 **cannot stream raw wideband baseband IQ samples** or tune across Sub-GHz/HF/UHF bands (e.g., `24 MHz–6 GHz`).

## 2. Host-Side Honest SDR Integration ([`desktop/serial_comm.py`](../../desktop/serial_comm.py))
- `GhostSDRInterface.get_info()` explicitly probes host CLI tooling (`hackrf_sweep`/`hackrf_info` and `rtl_power`/`rtl_test`).
- When neither external USB SDR hardware nor drivers are attached, the Desktop `SDR Interface` view displays `"None on Host (ESP32-S3 is NOT an SDR)"` and disables false tuning telemetry instead of simulating fake RF waterfalls.
