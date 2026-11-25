#ifndef HARDWARE_BOARD_CONFIG_H
#define HARDWARE_BOARD_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/spi_master.h"
#include "driver/uart.h"
#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

/* ========================================================================= */
/* 1. TARGET BOARD IDENTITY & SYSTEM RATINGS                                 */
/* ========================================================================= */

#define BOARD_TARGET_NAME           "7Semi ESP32-S3-WROOM-1-N16R8"
#define BOARD_FIRMWARE_REVISION     "GodMode-V2"
#define BOARD_SOC_NAME              "ESP32-S3 (Xtensa Dual-Core LX7 @ 240 MHz)"
#define BOARD_FLASH_SIZE_MB         16
#define BOARD_FLASH_MODE            "DIO @ 80 MHz"
#define BOARD_PSRAM_SIZE_MB         8
#define BOARD_PSRAM_TYPE            "Octal AP_3v3 @ 80 MHz DDR"
#define BOARD_TOTAL_GPIOS           49

/* ========================================================================= */
/* 2. CANONICAL PERIPHERAL PIN DEFINITIONS (SINGLE SOURCE OF TRUTH)          */
/* ========================================================================= */

/* --- Primary I2C Bus (Sensors, RTC, OLED expansions) --- */
#define BOARD_I2C_PORT              I2C_NUM_0
#define BOARD_PIN_I2C_SDA           GPIO_NUM_8
#define BOARD_PIN_I2C_SCL           GPIO_NUM_9
#define BOARD_I2C_FREQ_HZ           400000
#define BOARD_I2C_TIMEOUT_MS        100

/* --- Dedicated RF SPI Bus (SPI2_HOST / FSPI: NRF24L01+PA+LNA) --- */
#define BOARD_RF_SPI_HOST           SPI2_HOST
#define BOARD_PIN_RF_SPI_CS         GPIO_NUM_10
#define BOARD_PIN_RF_SPI_MOSI       GPIO_NUM_11
#define BOARD_PIN_RF_SPI_SCK        GPIO_NUM_12
#define BOARD_PIN_RF_SPI_MISO       GPIO_NUM_13
#define BOARD_PIN_RF_CE             GPIO_NUM_14
#define BOARD_PIN_RF_IRQ            GPIO_NUM_NC   /* Unconnected: Polled via SPI */
#define BOARD_RF_SPI_CLOCK_HZ       8000000

/* --- Dedicated Display SPI Bus (SPI3_HOST / HSPI: 1.8" ST7735S 128x160 TFT) --- */
#define BOARD_DISP_SPI_HOST         SPI3_HOST
#define BOARD_PIN_DISP_MOSI         GPIO_NUM_15
#define BOARD_PIN_DISP_SCK          GPIO_NUM_16
#define BOARD_PIN_DISP_CS           GPIO_NUM_5
#define BOARD_PIN_DISP_DC           GPIO_NUM_6
#define BOARD_PIN_DISP_RST          GPIO_NUM_7
#define BOARD_PIN_DISP_BCKL         GPIO_NUM_NC   /* Hardwired to 3.3V rail */
#define BOARD_DISP_WIDTH            128
#define BOARD_DISP_HEIGHT           160
#define BOARD_DISP_SPI_CLOCK_HZ     20000000

/* --- High-Speed Serial UART1 (GPS / Telemetry / Modem) --- */
#define BOARD_UART1_PORT            UART_NUM_1
#define BOARD_PIN_UART1_TX          GPIO_NUM_43
#define BOARD_PIN_UART1_RX          GPIO_NUM_44
#define BOARD_UART1_DEFAULT_BAUD    115200

/* --- Auxiliary Serial UART2 (Expansion Interface) --- */
#define BOARD_UART2_PORT            UART_NUM_2
#define BOARD_PIN_UART2_TX          GPIO_NUM_17
#define BOARD_PIN_UART2_RX          GPIO_NUM_18
#define BOARD_UART2_DEFAULT_BAUD    115200

/* --- Native USB-OTG Subsystem (RESTRICTED: NEVER REASSIGN) --- */
#define BOARD_PIN_USB_DM            GPIO_NUM_19   /* USB Full-Speed D- */
#define BOARD_PIN_USB_DP            GPIO_NUM_20   /* USB Full-Speed D+ */

/* --- Onboard Addressable RGB Indicator (WS2812B NeoPixel) --- */
#define BOARD_PIN_RGB_LED           GPIO_NUM_48
#define BOARD_RGB_LED_COUNT         1
#define BOARD_RGB_LED_COLOR_ORDER   0             /* 0 = GRB (WS2812B default) */

/* --- Auxiliary & System Interrupts --- */
#define BOARD_PIN_RTC_INT           GPIO_NUM_21   /* Auxiliary / RTC INT */
#define BOARD_PIN_AUX_GPIO          GPIO_NUM_47   /* Auxiliary General Purpose */

/* ========================================================================= */
/* 3. PLANNED SD CARD PIN MAPPINGS (CONFLICT-FREE AUTHORITY)                 */
/* ========================================================================= */

/* --- Mode 1: Dedicated 1-Bit SDMMC Mode (RECOMMENDED ARCHITECTURE) --- */
#define BOARD_PIN_SDMMC_CLK         GPIO_NUM_1
#define BOARD_PIN_SDMMC_CMD         GPIO_NUM_2
#define BOARD_PIN_SDMMC_D0          GPIO_NUM_4
#define BOARD_PIN_SDMMC_CD          GPIO_NUM_47   /* Optional Card Detect */

/* --- Mode 2: Dedicated SPI Mode on Free GPIOs --- */
#define BOARD_PIN_SD_SPI_CS         GPIO_NUM_4
#define BOARD_PIN_SD_SPI_SCK        GPIO_NUM_1
#define BOARD_PIN_SD_SPI_MOSI       GPIO_NUM_2
#define BOARD_PIN_SD_SPI_MISO       GPIO_NUM_47

/* --- Mode 3: Shared Display SPI3 Mode --- */
#define BOARD_PIN_SD_SHARED_CS      GPIO_NUM_4
#define BOARD_PIN_SD_SHARED_SCK     GPIO_NUM_16   /* Shared with Display */
#define BOARD_PIN_SD_SHARED_MOSI    GPIO_NUM_15   /* Shared with Display */
#define BOARD_PIN_SD_SHARED_MISO    GPIO_NUM_47   /* Dedicated return line */

/* ========================================================================= */
/* 4. SILICON RESTRICTION MASKS & LIMITS                                     */
/* ========================================================================= */

#define BOARD_RESTRICTED_OCTAL_PSRAM_START  33
#define BOARD_RESTRICTED_OCTAL_PSRAM_END    38
#define BOARD_RESTRICTED_FLASH_START        27
#define BOARD_RESTRICTED_FLASH_END          32

/* Strapping Pins: GPIO 0, 3, 45, 46 */
#define BOARD_STRAPPING_PINS_MASK   ((1ULL << 0) | (1ULL << 3) | (1ULL << 45) | (1ULL << 46))

/* Input-Only Silicon Pins (No output driver) */
#define BOARD_INPUT_ONLY_PINS_MASK  (1ULL << 46)

/* Octal PSRAM Bus Mask (GPIO 33-38) */
#define BOARD_OCTAL_PSRAM_PINS_MASK ((1ULL << 33) | (1ULL << 34) | (1ULL << 35) | \
                                     (1ULL << 36) | (1ULL << 37) | (1ULL << 38))

/* Native USB Pins Mask (GPIO 19, 20) */
#define BOARD_NATIVE_USB_PINS_MASK  ((1ULL << 19) | (1ULL << 20))

/* ========================================================================= */
/* 5. DATA TYPES & ENUMERATIONS                                              */
/* ========================================================================= */

/**
 * @brief Identifiers for physical peripherals and software managers.
 */
typedef enum {
    PERIPH_ID_NONE = 0,
    PERIPH_ID_SYSTEM,
    PERIPH_ID_USB_NATIVE,
    PERIPH_ID_OCTAL_PSRAM,
    PERIPH_ID_SPI_FLASH,
    PERIPH_ID_I2C0,
    PERIPH_ID_SPI2_NRF24,
    PERIPH_ID_SPI3_DISPLAY,
    PERIPH_ID_UART1,
    PERIPH_ID_UART2,
    PERIPH_ID_RGB_NEOPIXEL,
    PERIPH_ID_SD_CARD,
    PERIPH_ID_RTC,
    PERIPH_ID_IR,
    PERIPH_ID_CUSTOM,
    PERIPH_ID_MAX
} periph_id_t;

/**
 * @brief Pin capability, direction, and restriction flags.
 */
typedef enum {
    PIN_FLAG_NONE          = 0,
    PIN_FLAG_OUTPUT        = (1 << 0),
    PIN_FLAG_INPUT         = (1 << 1),
    PIN_FLAG_BIDIR         = (PIN_FLAG_OUTPUT | PIN_FLAG_INPUT),
    PIN_FLAG_OPEN_DRAIN    = (1 << 2),
    PIN_FLAG_PULLUP        = (1 << 3),
    PIN_FLAG_PULLDOWN      = (1 << 4),
    PIN_FLAG_STRAPPING     = (1 << 5),
    PIN_FLAG_INPUT_ONLY    = (1 << 6),
    PIN_FLAG_PSRAM_BUS     = (1 << 7),
    PIN_FLAG_USB_DEDICATED = (1 << 8),
    PIN_FLAG_JTAG          = (1 << 9),
    PIN_FLAG_LOCKED        = (1 << 10),
    PIN_FLAG_SHARED_BUS    = (1 << 11)
} pin_flags_t;

/**
 * @brief Hardware presence state enumeration.
 */
typedef enum {
    HW_PRESENCE_UNKNOWN = 0,
    HW_PRESENCE_AVAILABLE,
    HW_PRESENCE_DISCONNECTED,
    HW_PRESENCE_FAULT,
    HW_PRESENCE_DISABLED
} hw_presence_t;

/**
 * @brief Structure representing an individual pin allocation record.
 */
typedef struct {
    gpio_num_t  pin;
    const char *name;
    periph_id_t owner;
    const char *owner_name;
    pin_flags_t flags;
    bool        is_allocated;
    bool        is_locked;
} pin_allocation_t;

/**
 * @brief Structure tracking peripheral registration and status.
 */
typedef struct {
    periph_id_t   id;
    const char   *name;
    hw_presence_t presence;
    esp_err_t     last_error;
    uint64_t      pin_mask;
} periph_info_t;

/* ========================================================================= */
/* 6. HARDWARE ABSTRACTION & PIN REGISTRY API PROTOTYPES                     */
/* ========================================================================= */

/**
 * @brief Initialize the Board Configuration and Pin Registry.
 *
 * Pre-populates all 49 ESP32-S3 GPIO capabilities, permanently locks
 * restricted pins (Native USB 19/20, Octal PSRAM 33-38, Flash 27-32), and
 * registers static 7Semi peripherals (I2C0, SPI2, SPI3, UART1, UART2, RGB).
 *
 * @return ESP_OK on success, or error code on failure.
 */
esp_err_t board_init(void);

/**
 * @brief Check if a GPIO number physically exists on ESP32-S3 silicon.
 *
 * @param pin GPIO pin number to validate.
 * @return true if valid (0-21, 33-48), false otherwise.
 */
bool board_pin_is_valid(gpio_num_t pin);

/**
 * @brief Check whether a pin is restricted (USB, PSRAM, Flash, Strapping).
 *
 * @param pin GPIO pin number to check.
 * @param reason_out Optional pointer to receive a human-readable restriction reason.
 * @return true if restricted/forbidden, false if generally available.
 */
bool board_pin_is_restricted(gpio_num_t pin, const char **reason_out);

/**
 * @brief Check if a pin is currently allocated to an active peripheral.
 *
 * @param pin GPIO pin number to check.
 * @param owner_out Optional pointer to receive the current owner peripheral ID.
 * @return true if allocated, false if free.
 */
bool board_pin_is_allocated(gpio_num_t pin, periph_id_t *owner_out);

/**
 * @brief Register and reserve a GPIO pin for a peripheral.
 *
 * Validates pin availability, checks against restriction masks, verifies
 * direction compatibility (e.g. rejects output mode on input-only GPIO 46),
 * and prevents conflicting double-allocations.
 *
 * @param pin GPIO pin number.
 * @param owner Peripheral ID requesting allocation.
 * @param owner_name Descriptive string for diagnostics.
 * @param flags Capability and direction flags (PIN_FLAG_*).
 * @return ESP_OK on success; ESP_ERR_INVALID_STATE if already allocated;
 *         ESP_ERR_NOT_SUPPORTED if restricted.
 */
esp_err_t board_register_pin(gpio_num_t pin, periph_id_t owner, const char *owner_name, pin_flags_t flags);

/**
 * @brief Release a previously registered pin back to the free pool.
 *
 * System-locked pins cannot be released.
 *
 * @param pin GPIO pin number to release.
 * @param owner Peripheral ID releasing the pin.
 * @return ESP_OK on success, or error code.
 */
esp_err_t board_release_pin(gpio_num_t pin, periph_id_t owner);

/**
 * @brief Check if a proposed pin allocation would cause a hardware conflict.
 *
 * @param pin GPIO pin number.
 * @param requesting_owner Peripheral ID requesting access.
 * @return ESP_OK if no conflict; ESP_ERR_INVALID_STATE if conflict exists.
 */
esp_err_t board_check_conflict(gpio_num_t pin, periph_id_t requesting_owner);

/**
 * @brief Register a peripheral and track its allocated pins.
 *
 * @param id Peripheral ID.
 * @param name Peripheral name.
 * @param pin_mask 64-bit mask of pins used by this peripheral.
 * @return ESP_OK on success.
 */
esp_err_t board_register_peripheral(periph_id_t id, const char *name, uint64_t pin_mask);

/**
 * @brief Get the hardware presence status of a peripheral.
 *
 * @param id Peripheral ID.
 * @return Hardware presence state (HW_PRESENCE_*).
 */
hw_presence_t board_get_periph_presence(periph_id_t id);

/**
 * @brief Update the hardware presence status and last error code of a peripheral.
 *
 * @param id Peripheral ID.
 * @param presence Presence state.
 * @param last_error ESP error code or ESP_OK.
 */
void board_set_periph_presence(periph_id_t id, hw_presence_t presence, esp_err_t last_error);

/**
 * @brief Get read-only pointer to a pin's allocation record.
 *
 * @param pin GPIO pin number.
 * @return Pointer to allocation struct, or NULL if invalid pin.
 */
const pin_allocation_t *board_get_pin_allocation(gpio_num_t pin);

/**
 * @brief Dump the complete pin allocation and restriction table to the serial console.
 */
void board_dump_pinout_table(void);

/**
 * @brief Perform a non-destructive hardware presence check for NRF24L01+ on SPI2.
 *
 * Probes the CONFIG register (0x00). If read value is not 0x00 and not 0xFF,
 * hardware presence is confirmed.
 *
 * @return HW_PRESENCE_AVAILABLE or HW_PRESENCE_DISCONNECTED.
 */
hw_presence_t board_check_nrf24_presence(void);

/**
 * @brief Perform a non-destructive I2C probe for a device address on Primary I2C.
 *
 * @param addr 7-bit I2C device address.
 * @return HW_PRESENCE_AVAILABLE if ACK received; HW_PRESENCE_DISCONNECTED if NACK.
 */
hw_presence_t board_check_i2c_presence(uint8_t addr);

/**
 * @brief Scan Primary I2C bus (0x08..0x77) for responding devices.
 *
 * @param found_addrs Optional output buffer for detected 7-bit addresses.
 * @param max_addrs Capacity of found_addrs.
 * @return Number of ACKed devices, or -1 if bus could not be initialized.
 */
int board_scan_i2c_bus(uint8_t *found_addrs, int max_addrs);

/**
 * @brief Perform a non-destructive display hardware presence check.
 *
 * @return HW_PRESENCE_AVAILABLE or HW_PRESENCE_DISCONNECTED.
 */
hw_presence_t board_check_display_presence(void);

/**
 * @brief Convert a peripheral ID to a human-readable string.
 *
 * @param id Peripheral ID.
 * @return String name.
 */
const char *board_periph_id_to_name(periph_id_t id);

/**
 * @brief Convert a hardware presence code to a human-readable string.
 *
 * @param status Presence status.
 * @return String description.
 */
const char *board_hw_presence_to_str(hw_presence_t status);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_BOARD_CONFIG_H */
