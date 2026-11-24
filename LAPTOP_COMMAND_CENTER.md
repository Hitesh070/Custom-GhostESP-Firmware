# GhostESP God Mode V3 – Laptop Command Center

The **GhostESP Laptop Command Center** is a standalone, lightweight, cyberpunk-themed desktop operations station engineered for real-time telemetry, RF inspection, wireless attack monitoring, and SDR visualization.

---

## 1. Architecture & Design

- **Framework**: Python 3.10+ / `tkinter` (Native, zero external UI framework overhead).
- **Communication Layer**: Dedicated background thread managing bidirectional UART / USB CDC serial communication (`desktop/serial_comm.py`).
- **Database Subsystem**: Thread-safe SQLite engine logging all detected access points, rogue warnings, and deauthentication frame bursts (`desktop/database.py`).
- **Export Formats**: One-click CSV and JSON exports for offensive security reporting and lab evidence preservation.
- **Hardware Agnostic**: Fully compatible with CP2102/CH343 serial ports and ESP32-S3 Native USB CDC ports.

---

## 2. Capabilities & Interface Tabs

### 1. Dashboard & System Telemetry
- Real-time battery voltage, active profile indicator, FreeRTOS CPU load, and internal SRAM/PSRAM memory gauges.
- Live system status cards displaying peripheral health (I2C, SPI2 NRF24, SPI3 Display, WS2812B RGB NeoPixel).
- Quick-action buttons for triggering automated hardware self-tests (`selftest`) and rebooting into specialized profiles.

### 2. Wi-Fi Security Research Suite
- **Passive Rogue AP & Evil Twin Detection**: Real-time table streaming multi-vector threat assessments (OUI mismatch, security downgrade, channel hop, RSSI delta).
- **Deauthentication Storm Monitor**: Frame rate counter, transmitter/victim MAC mapping, reason code decoding, and rate alarm triggers.

### 3. BLE Reconnaissance Lab
- Device scanner table displaying detected UUIDs, RSSI signal meters, MAC addresses, and manufacturer payload identifiers.
- Live BLE packet stream inspector.

### 4. NRF24 RF Lab & Spectrum Sweep
- **24-Register Bitfield Inspector**: Live register decodes showing RX/TX pipes, RF channel, PA output power, and auto-acknowledgment status.
- **126-Channel RPD Carrier Sweep**: Live canvas rendering of RF energy levels across 2.400 GHz – 2.525 GHz.

### 5. Threat Timeline & Incident Feed
- Chronological, color-coded alert feed (INFO, WARN, CRITICAL).
- Search and filter by MAC address, SSID, or threat vector.

### 6. SDR Interface & Dual-Receiver Abstraction
- Abstracted host-side SDR interface (`desktop/modules/sdr_abstraction.py`) supporting RTL-SDR and HackRF for wideband waterfall correlation alongside GhostESP channel sweeps.

### 7. Interactive Serial Terminal
- Full ANSI escape sequence support and command history.
- Quick macro shortcuts for God Mode commands (`roguedetect start`, `deauthmon status`, `selftest`, `ghost matrix`).

---

## 3. Quickstart & Installation

```bash
# Navigate to desktop companion directory
cd desktop

# Install dependencies (only pyserial required; tkinter is bundled with standard Python)
pip install -r requirements.txt

# Launch Command Center
python app.py
```
