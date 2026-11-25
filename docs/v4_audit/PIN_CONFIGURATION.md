# Canonical Pin Configuration (7Semi ESP32-S3-WROOM-1-N16R8)

Source of truth: [`include/hardware/board_config.h`](../../include/hardware/board_config.h) and `sdkconfig`.

| GPIO(s) | Bus / Function | Signal Mapping | Conflict / Silicon Constraint |
|---|---|---|---|
| `GPIO 1, 2, 4, 47` | SD Card (`SPI` / `SDMMC`) | Mode 1 MMC: `CLK=1, CMD=2, D0=4, CD=47`; Mode 2 SPI: `CS=4, SCK=1, MOSI=2, MISO=47` | Mode 3 shared SPI3 sets `CS=4, SCK=16, MOSI=15, MISO=47` |
| `GPIO 5, 6, 7, 15, 16` | `SPI3_HOST` Display (`ST7735S`) | `CS=5, DC=6, RST=7, MOSI=15, SCK=16, BCKL=3.3V` | Write-only TFT (`MISO` absent) |
| `GPIO 8, 9` | `I2C_NUM_0` Primary | `SDA=8, SCL=9` @ `400 kHz` | Probed `0x08..0x77` in `selftest` |
| `GPIO 10, 11, 12, 13, 14` | `SPI2_HOST` RF (`NRF24L01+`) | `CS=10, MOSI=11, SCK=12, MISO=13, CE=14, IRQ=NC` | Polled over SPI @ `8 MHz` |
| `GPIO 17, 18` | `UART_NUM_2` (`esp_comm`) | `TX=17, RX=18` @ `115200` | Relocated in V4 away from display `GPIO 6/7` |
| `GPIO 19, 20` | Native USB-OTG PHY | `D-=19, D+=20` | **Silicon Locked** (`USB-JTAG` `COM5` / TinyUSB BadUSB / MSC) |
| `GPIO 21` | Auxiliary / RTC Interrupt | `BOARD_PIN_RTC_INT` | General expansion interrupt |
| `GPIO 27 - 32` | Internal SPI Flash | 16 MB `DIO @ 80 MHz` | **Silicon Restricted** — never allocate |
| `GPIO 33 - 38` | Internal Octal PSRAM | 8 MB `AP_3v3 @ 80 MHz DDR` | **Silicon Restricted** — never allocate |
| `GPIO 43, 44` | `UART_NUM_1` Telemetry / GPS | `TX=43, RX=44` @ `115200` | CP210x `COM6` / GPS NMEA RX (`44`) |
| `GPIO 48` | Onboard NeoPixel RGB | `WS2812B` GRB LED | Active status indicator |
| `GPIO 0, 3, 45, 46` | Strapping Pins | Boot/JTAG/VDD_SPI strapping (`46` input-only) | Output mode rejected on `GPIO 46` by `board_register_pin()` |
