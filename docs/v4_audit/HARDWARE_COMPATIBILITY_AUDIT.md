# Hardware Compatibility & Pin Registry Audit (7Semi ESP32-S3-WROOM-1-N16R8)

## 1. Silicon & Memory Verification
- **MCU:** ESP32-S3 (QFN56, rev v0.2), 240 MHz Xtensa LX7 (Dual Core silicon, `CONFIG_FREERTOS_UNICORE=y` intentionally retained for shared scan-table safety)
- **Flash:** 16 MB (`--flash-size 16MB`, DIO @ 80 MHz)
- **PSRAM:** 8 MB Embedded Octal PSRAM (`AP_3v3`) — verified via `1 MB` runtime allocation test (`selftest`) and `temp json` free heap reporting (`~8 MB` free)
- **Dual USB Interfaces:**
  - `COM5`: Native USB-Serial/JTAG (`303A:1001`, GPIO19/20)
  - `COM6`: CP210x Hardware UART Bridge (UART0 GPIO43 TX / GPIO44 RX)

## 2. Verified Pin Authority Table ([`include/hardware/board_config.h`](file:///c:/Users/hites/OneDrive/Documents/Projects/Custom%20GhostESP%20Firmware/include/hardware/board_config.h))

| Peripheral / Domain | Allocated GPIOs | Conflict Fixes Applied in V4 | Live Bench Status |
|---|---|---|---|
| **SpiFlash / Octal PSRAM** | `GPIO 27-32` / `33-38` | Silicon-locked in `board_config.c` | Operational (Verified) |
| **USB-OTG / Serial-JTAG** | `GPIO 19 (D-)`, `GPIO 20 (D+)` | Removed from SD SPI (`MISO=19->47`) and SDMMC S3 pins (`19/20->1/4`) | Operational (`COM5` Verified) |
| **UART1 Console** | `GPIO 43 (TX)`, `GPIO 44 (RX)` | `CONFIG_GPS_UART_RX_PIN` aligned to `44` (moved off `17`) | Operational (`COM6` Verified) |
| **UART2 / ESP Comm Peer** | `GPIO 17 (TX)`, `GPIO 18 (RX)` | Moved `settings_manager.c` defaults off `TX=6 / RX=7` (Display collision) | Configured / Idle |
| **SPI3 TFT LCD (ST7735S)** | `CS=5, DC=6, RST=7, MOSI=15, SCK=16` | Eliminated ESP Comm `GPIO 6/7` collision | Pins Locked (External MISO absent; `HW REQUIRED`) |
| **SPI2 RF (NRF24L01+PA+LNA)** | `CS=10, MOSI=11, SCK=12, MISO=13, CE=14` | Releases CS/SPI device when unresponsive; eliminated GPIO14 conflict warning | Disconnected on bench (`CONFIG=0x00`, `HW REQUIRED`) |
| **Primary I2C (I2C0)** | `SDA=8, SCL=9` | Real `board_scan_i2c_bus()` (`0x08..0x77`) added in V4 | Configured (`0 ACKs` on bench -> `HW REQUIRED`) |
| **SD Card (SPI)** | `CS=4, CLK=1, MISO=47, MOSI=2` | Fixed `sdkconfig` (`18/19/23` -> `1/47/2`) avoiding USB `19` and unbonded `23` | No card inserted (`ESP_ERR_INVALID_RESPONSE`, `HW REQUIRED`) |
| **WS2812B NeoPixel RGB** | `GPIO 48` | Verified output-capable onboard pin | Operational (Verified) |
| **IR Blaster (Optional)** | `CONFIG_HAS_INFRARED` disabled | `infrared_manager.c` hardened (zero-init structs, null-checked encoder, idle-low) | Disabled in default config (`irpin` CLI present) |
