# GhostESP God Mode V3 – Laptop Command Center

The **GhostESP Laptop Command Center** is a dedicated desktop operations suite and companion application for the GhostESP firmware on the **7Semi ESP32-S3-WROOM-1-N16R8**.

---

## 1. Architecture & Design Principles

- **Host-Side Processing:** All intensive visualization, database persistence, and wideband SDR spectral digitizing are kept on the host laptop, leaving the ESP32-S3 microcontroller unburdened.
- **Bi-Directional Telemetry:** Operates over the ESP32-S3 native USB serial port (`115200` baud default).
- **Persistent SQLite Storage:** Automatically logs AP discoveries, rogue AP events, and deauth storms into `ghostesp_sessions.db`.
- **Export Capabilities:** One-click export to standard CSV and JSON.

---

## 2. Quick Start

### Prerequisites
- Python 3.10+
- `pyserial` (installed in Python environment)

### Install Dependencies:
```bash
cd desktop
pip install -r requirements.txt
```

### Launch the Application:
```bash
python app.py
```

---

## 3. Operations & Tabs

1. **Dashboard:** Live hardware status, memory meters, and fast command dispatch buttons.
2. **Wi-Fi Lab:** Real-time AP table, channel occupancy, and Rogue AP / Evil Twin alert monitoring.
3. **BLE Lab:** BLE peripheral node tracking.
4. **NRF24 RF:** Direct SPI2 register inspector, 126-channel spectrum activity histogram, and link benchmark.
5. **Timeline:** Cross-subsystem unified event stream.
6. **SDR Interface:** Extensible abstraction layer for host-attached SDRs (RTL-SDR v4, HackRF One).
7. **Console:** Interactive bidirectional terminal for CLI commands.
