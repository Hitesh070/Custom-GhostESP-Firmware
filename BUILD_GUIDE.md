# Comprehensive Build, Flash & Configuration Guide
## GhostESP God Mode V2 – 7Semi ESP32-S3 Devkit (N16R8)

**Target Hardware:** 7Semi ESP32-S3-WROOM-1-N16R8 (Dual USB-C, 16 MB Flash, 8 MB Octal PSRAM)  
**Framework Version:** Espressif IoT Development Framework (ESP-IDF) v6.1.1  
**Build Toolchain:** `xtensa-esp-elf` (GCC 13.2 / ESP-IDF 6.1)  
**Host Environment:** Windows 11 (PowerShell Core / Windows PowerShell)  
**Document Version:** 2.0.0  

---

## 1. Prerequisites & Host Toolchain Setup

### 1.1 Toolchain Paths
GhostESP God Mode V2 relies on the official ESP-IDF v6.1.1 environment installed at:
- **ESP-IDF Root:** `C:\esp\v6.1.1\esp-idf`
- **Espressif Tools:** `C:\Espressif\tools`
- **Python Virtual Environment:** `C:\Espressif\tools\python\v6.1.1\venv`
- **PowerShell Profile Helper:** `C:\Espressif\tools\Microsoft.v6.1.1.PowerShell_profile.ps1`

### 1.2 Activating the Environment
To execute any IDF command in PowerShell on Windows, source the profile in your session:
```powershell
. C:\Espressif\tools\Microsoft.v6.1.1.PowerShell_profile.ps1
```
Or run commands in an isolated session:
```powershell
powershell -ExecutionPolicy Bypass -NoProfile -Command ". C:\Espressif\tools\Microsoft.v6.1.1.PowerShell_profile.ps1; idf.py --version"
```

---

## 2. Target Board Configuration & SDK Options

The target configuration baseline is stored in [`configs/sdkconfig.default.esp32s3`](configs/sdkconfig.default.esp32s3).

### Key Architectural Configurations:
1. **Target Silicon & Clocking:**
   - `CONFIG_IDF_TARGET="esp32s3"`
   - `CONFIG_ESP32S3_DEFAULT_CPU_FREQ_240=y` (240 MHz Dual-Core LX7)
2. **Flash Memory (16 MB):**
   - `CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y`
   - `CONFIG_ESPTOOLPY_FLASHMODE_DIO=y`
   - `CONFIG_ESPTOOLPY_FLASHFREQ_80M=y`
3. **Octal PSRAM (8 MB):**
   - `CONFIG_SPIRAM=y`
   - `CONFIG_SPIRAM_MODE_OCT=y`
   - `CONFIG_SPIRAM_SPEED_80M=y`
   - `CONFIG_SPIRAM_FETCH_INSTRUCTIONS=y`
   - `CONFIG_SPIRAM_RODATA=y`
4. **Physical Display (ST7735S 128×160 on SPI3):**
   - `CONFIG_WITH_SCREEN=y`
   - `CONFIG_TFT_WIDTH=128`
   - `CONFIG_TFT_HEIGHT=160`
   - `CONFIG_LV_TFT_DISPLAY_CONTROLLER_ST7735S=y`
   - `CONFIG_LV_TFT_DISPLAY_SPI3_HOST=y`
   - `CONFIG_LV_DISP_SPI_MOSI=15`
   - `CONFIG_LV_DISP_SPI_CLK=16`
   - `CONFIG_LV_DISP_SPI_CS=5`
   - `CONFIG_LV_DISP_PIN_DC=6`
   - `CONFIG_LV_DISP_PIN_RST=7`
   - `CONFIG_LV_DISP_PIN_BCKL=-1` (Hardwired to 3.3V)
5. **Dedicated RF SPI (NRF24L01+ on SPI2):**
   - `CONFIG_NRF24_MOSI_PIN=11`
   - `CONFIG_NRF24_MISO_PIN=13`
   - `CONFIG_NRF24_SCK_PIN=12`
   - `CONFIG_NRF24_CS_PIN=10`
   - `CONFIG_NRF24_CE_PIN=14`
   - `CONFIG_NRF24_SPI_CLOCK_HZ=8000000`
6. **TinyUSB BadUSB HID:**
   - `CONFIG_TINYUSB_HID_COUNT=3` (Keyboard, Mouse, Gamepad)

---

## 3. Step-by-Step Compilation Workflow

### 3.1 Initial Configuration / Reconfiguration
When new C files or header directories are added under `main/` or `include/`:
```powershell
powershell -ExecutionPolicy Bypass -NoProfile -Command ". C:\Espressif\tools\Microsoft.v6.1.1.PowerShell_profile.ps1; idf.py reconfigure"
```

### 3.2 Building the Firmware
To perform a complete clean build of the firmware, bootloader, and partition table:
```powershell
powershell -ExecutionPolicy Bypass -NoProfile -Command ". C:\Espressif\tools\Microsoft.v6.1.1.PowerShell_profile.ps1; idf.py build"
```

### 3.3 Inspecting Build Output Sizes
Upon successful compilation, verify the generated binaries in the `build/` directory:
- `build/bootloader/bootloader.bin` (~21 KB, loaded at `0x0000`)
- `build/partition_table/partition-table.bin` (~3 KB, loaded at `0x8000`)
- `build/ghostesp.bin` (~3.6 MB, loaded at `0x10000`)

---

## 4. Flashing & Serial Deployment

The 7Semi ESP32-S3 Devkit provides two USB-C ports:
- **COM5:** Native USB-Serial/JTAG port (Direct on-chip USB OTG). Recommended for high-speed flashing and JTAG debugging.
- **COM6:** CP210x USB-to-UART bridge port (Connected to UART0: GPIO43/44). Ideal for secondary serial CLI monitoring.

### 4.1 Flashing via Native USB (COM5)
```powershell
powershell -ExecutionPolicy Bypass -NoProfile -Command ". C:\Espressif\tools\Microsoft.v6.1.1.PowerShell_profile.ps1; idf.py -p COM5 -b 460800 flash"
```

> [!IMPORTANT]
> **COM Port Locking:** Ensure no serial monitor, WebSerial dashboard (e.g., GhostESP Web Dashboard), or terminal holds COM5 open before flashing. If you encounter `PermissionError(13, 'Access is denied.')`, disconnect the serial monitor and retry.

### 4.2 Interactive Serial Monitoring
To monitor system logs directly after flashing:
```powershell
powershell -ExecutionPolicy Bypass -NoProfile -Command ". C:\Espressif\tools\Microsoft.v6.1.1.PowerShell_profile.ps1; idf.py -p COM5 monitor"
```
*(Press `Ctrl+]` to exit the IDF monitor).*

---

## 5. Partition Table Architecture (16 MB Flash)

GhostESP uses a custom 16 MB partition table supporting dual OTA updates, LittleFS/SPIFFS file storage, core dumps, and factory calibration:

```text
# Name,       Type, SubType, Offset,   Size,     Flags
nvs,          data, nvs,     0x9000,   0x6000,
otadata,      data, ota,     0xf000,   0x2000,
phy_init,     data, phy,     0x11000,  0x1000,
factory,      app,  factory, 0x20000,  0x500000,
ota_0,        app,  ota_0,   0x520000, 0x500000,
ota_1,        app,  ota_1,   0xa20000, 0x500000,
coredump,     data, coredump,0xf20000, 0x10000,
storage,      data, spiffs,  0xf30000, 0xd0000,
```

- Each application partition (`factory`, `ota_0`, `ota_1`) is allocated **5 MB (0x500000)**, providing ample room for comprehensive features, font tables, and embedded assets.
- `storage` provides ~832 KB of onboard non-volatile SPIFFS storage for PCAP recordings and Evil Portal templates when an SD card is not inserted.
