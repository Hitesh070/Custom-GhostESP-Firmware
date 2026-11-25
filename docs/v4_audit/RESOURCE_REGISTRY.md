# Hardware & Peripheral Resource Registry

Defined in [`include/hardware/board_config.h`](../../include/hardware/board_config.h) & [`main/hardware/board_config.c`](../../main/hardware/board_config.c).

## 1. Registered `periph_id_t` Owners
- `PERIPH_ID_SYSTEM`, `PERIPH_ID_USB_NATIVE`, `PERIPH_ID_OCTAL_PSRAM`, `PERIPH_ID_SPI_FLASH`
- `PERIPH_ID_I2C0`, `PERIPH_ID_SPI2_NRF24`, `PERIPH_ID_SPI3_DISPLAY`
- `PERIPH_ID_UART1`, `PERIPH_ID_UART2`, `PERIPH_ID_RGB_NEOPIXEL`, `PERIPH_ID_SD_CARD`, `PERIPH_ID_RTC`, `PERIPH_ID_IR`, `PERIPH_ID_CUSTOM`

## 2. Pin Capability & Protection Flags (`pin_flags_t`)
- Direction & drive: `PIN_FLAG_OUTPUT`, `PIN_FLAG_INPUT`, `PIN_FLAG_BIDIR`, `PIN_FLAG_OPEN_DRAIN`, `PIN_FLAG_PULLUP`, `PIN_FLAG_PULLDOWN`
- Hardware locks: `PIN_FLAG_STRAPPING`, `PIN_FLAG_INPUT_ONLY` (blocks output requests on `GPIO 46`), `PIN_FLAG_PSRAM_BUS`, `PIN_FLAG_USB_DEDICATED`, `PIN_FLAG_JTAG`, `PIN_FLAG_LOCKED`, `PIN_FLAG_SHARED_BUS`

## 3. Honest Presence Reporting (`hw_presence_t`)
- External buses are NEVER reported `HW_PRESENCE_AVAILABLE` blindly:
  - `board_scan_i2c_bus(0x08..0x77)` verifies real I2C ACKs (`0` ACKs -> `HW REQUIRED`).
  - `board_check_nrf24_presence()` reads NRF24 `REG_CONFIG (0x00)`; `0x00`/`0xFF` returns `HW_PRESENCE_DISCONNECTED` (`HW REQUIRED`).
  - USB OTG Native PHY mutual exclusion guarantees `badusb_manager`, `usb_keyboard_manager`, and `usb_msc_manager` never contend over `GPIO 19/20`.
