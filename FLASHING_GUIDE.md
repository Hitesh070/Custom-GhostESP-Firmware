# GhostESP God Mode V3 – Flashing and Recovery Guide

This guide provides authoritative flashing instructions for the **7Semi ESP32-S3-WROOM-1-N16R8** development board (16 MB Flash, 8 MB Octal PSRAM AP_3v3).

---

## 1. Hardware Overview & Connection

The 7Semi ESP32-S3 board features dual USB-C ports:
- **UART Port (CP2102/CH343)**: Default flashing and standard serial console interface (115200 baud). Recommended for initial programming and unbrick recovery.
- **Native USB Port (ESP32-S3 JTAG/CDC)**: Direct USB connection using GPIO19 (D-) and GPIO20 (D+). Used for high-speed USB CDC, BadUSB emulation, and web flasher communication.

---

## 2. Flashing via ESP-IDF CLI (`idf.py`)

If you have ESP-IDF v6.1.1 configured locally:

```powershell
# Open IDF PowerShell environment
. C:\Espressif\tools\Microsoft.v6.1.1.PowerShell_profile.ps1

# Navigate to project repository
cd "c:\Users\hites\OneDrive\Documents\Projects\Custom GhostESP Firmware"

# Flash complete firmware suite to specified COM port (e.g., COM3)
idf.py -p COM3 -b 921600 flash monitor
```

---

## 3. Standalone Flashing via `esptool.py`

You do not need the full ESP-IDF toolchain installed to flash pre-compiled binaries. Use `esptool.py` (available via Python `pip install esptool`):

### Binary Locations & Memory Offsets (16 MB Flash)

| Binary File | Target Flash Offset | Description |
| :--- | :--- | :--- |
| `build/bootloader/bootloader.bin` | `0x0000` | Second-stage bootloader |
| `build/partition_table/partition-table.bin` | `0x8000` | Custom 16 MB partition table |
| `build/ota_data_initial.bin` | `0xd000` | OTA data sector (slot 0 boot) |
| `build/Ghost_ESP_IDF.bin` | `0x10000` | God Mode V3 Application firmware |

### Unified `esptool.py` Flash Command

```bash
esptool.py --chip esp32s3 -p <PORT> -b 921600 \
  --before default_reset --after hard_reset write_flash -z \
  --flash_mode dio --flash_freq 80m --flash_size 16MB \
  0x0000  build/bootloader/bootloader.bin \
  0x8000  build/partition_table/partition-table.bin \
  0xd000  build/ota_data_initial.bin \
  0x10000 build/Ghost_ESP_IDF.bin
```

---

## 4. Web Flashing (GhostESP Web Flasher / Chrome)

You can flash directly from Chromium-based browsers (Chrome, Edge, Brave) via WebSerial:
1. Connect the ESP32-S3 via the UART USB-C port.
2. Navigate to the GhostESP Web Flasher.
3. Select `ESP32-S3 (16MB)` and choose the `Ghost_ESP_IDF.bin` binary.
4. Click **Connect** and select your COM port.
5. Click **Program** and wait for completion.

---

## 5. Recovery / Bootloader Force Mode

If the board enters a boot loop, fails to respond over UART, or has corrupted flash:

1. **Enter Bootloader Mode**:
   - Press and **HOLD** the `BOOT` button (GPIO0).
   - Momentarily press the `RESET` (EN) button.
   - Release the `BOOT` button after 1 second.
2. The ROM bootloader will output `waiting for download` on the UART console at 115200 baud.
3. **Erase Flash completely**:
   ```bash
   esptool.py --chip esp32s3 -p <PORT> erase_flash
   ```
4. Re-flash the full binary image as detailed in Section 3.
