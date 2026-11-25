# GhostESP God Mode V4 — Developer Manual

## 1. Firmware Build & Flash Workflow (`ESP-IDF v6.1.1`)
1. Activate ESP-IDF v6.1.1 environment (PowerShell): ensure `idf.py` and `esptool.py` are in `PATH`.
2. Build firmware:
   ```powershell
   idf.py build
   ```
3. Flash onto 7Semi ESP32-S3 (`COM5` Native USB-JTAG or `COM6` CP210x):
   ```powershell
   idf.py -p COM5 flash
   ```
4. Verify live telemetry: open serial monitor or launch GhostESP Command Center (`python desktop/app.py`).

## 2. Desktop Development & Testing
```powershell
pip install -r desktop/requirements.txt
pytest -v desktop/tests/test_app_protocol_db.py
python desktop/app.py
```

## 3. Coding Invariants Enforced in V4
- **PSRAM Allocation Discipline:** Keep `CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL=2048` so mbedTLS, LWIP, and NimBLE prefer PSRAM, preserving Internal DMA SRAM (`>= 45 KB` free) for SoftAP/HTTP/Wi-Fi RX.
- **No Unchecked VLA / Payload Copies:** Clamp all untrusted 802.11 SSIDs (`<= 32` bytes), BLE Adv payloads (`<= 31` bytes), and EAPOL buffers cleanly.
- **No `glog()` Under Short Mutexes:** Snapshot state inside `s_mutex`, release mutex, then call `glog()` so slow USB TX never blocks Wi-Fi/BLE callbacks.
- **No Fake Hardware Checks:** Every new bus probe must read/ACK hardware before returning `PASS` / `HW_PRESENCE_AVAILABLE`.
