#include "hardware/board_config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "BoardConfig";

/* ========================================================================= */
/* INTERNAL REGISTRY STATE                                                   */
/* ========================================================================= */

static pin_allocation_t s_pin_registry[BOARD_TOTAL_GPIOS];
static periph_info_t    s_periph_table[PERIPH_ID_MAX];
static bool             s_board_initialized = false;
static SemaphoreHandle_t s_board_mutex = NULL;

#define BOARD_LOCK()   do { if (s_board_mutex) xSemaphoreTakeRecursive(s_board_mutex, portMAX_DELAY); } while(0)
#define BOARD_UNLOCK() do { if (s_board_mutex) xSemaphoreGiveRecursive(s_board_mutex); } while(0)

/* ========================================================================= */
/* HELPER FUNCTIONS                                                          */
/* ========================================================================= */

const char *board_periph_id_to_name(periph_id_t id) {
    switch (id) {
        case PERIPH_ID_NONE:          return "None";
        case PERIPH_ID_SYSTEM:        return "System";
        case PERIPH_ID_USB_NATIVE:    return "Native USB-OTG";
        case PERIPH_ID_OCTAL_PSRAM:   return "Octal PSRAM";
        case PERIPH_ID_SPI_FLASH:     return "SPI Flash";
        case PERIPH_ID_I2C0:          return "Primary I2C (I2C0)";
        case PERIPH_ID_SPI2_NRF24:    return "NRF24L01+ (SPI2)";
        case PERIPH_ID_SPI3_DISPLAY:  return "ST7735S TFT (SPI3)";
        case PERIPH_ID_UART1:         return "UART1 (Telemetry)";
        case PERIPH_ID_UART2:         return "UART2 (Expansion)";
        case PERIPH_ID_RGB_NEOPIXEL:  return "WS2812B NeoPixel";
        case PERIPH_ID_SD_CARD:       return "SD Card";
        case PERIPH_ID_RTC:           return "RTC / Clock";
        case PERIPH_ID_IR:            return "Infrared (IR)";
        case PERIPH_ID_CUSTOM:        return "Custom Peripheral";
        default:                      return "Unknown";
    }
}

const char *board_hw_presence_to_str(hw_presence_t status) {
    switch (status) {
        case HW_PRESENCE_AVAILABLE:    return "Available (Connected)";
        case HW_PRESENCE_DISCONNECTED: return "Disconnected / Unpowered";
        case HW_PRESENCE_FAULT:        return "Hardware Fault";
        case HW_PRESENCE_DISABLED:     return "Disabled";
        case HW_PRESENCE_UNKNOWN:
        default:                       return "Unknown / Not Probed";
    }
}

bool board_pin_is_valid(gpio_num_t pin) {
    if ((int)pin < 0 || (int)pin >= BOARD_TOTAL_GPIOS) {
        return false;
    }
    /* ESP32-S3 QFN56 package: GPIO 22-25 are unrouted/unbonded */
    if (pin >= 22 && pin <= 25) {
        return false;
    }
    return true;
}

bool board_pin_is_restricted(gpio_num_t pin, const char **reason_out) {
    if (!board_pin_is_valid(pin)) {
        if (reason_out) *reason_out = "Unbonded / Non-existent GPIO";
        return true;
    }

    /* Octal PSRAM Bus: GPIO 33-38 */
    if (pin >= BOARD_RESTRICTED_OCTAL_PSRAM_START && pin <= BOARD_RESTRICTED_OCTAL_PSRAM_END) {
        if (reason_out) *reason_out = "Octal PSRAM Bus (FORBIDDEN)";
        return true;
    }

    /* Internal Flash Bus: GPIO 27-32 */
    if (pin >= BOARD_RESTRICTED_FLASH_START && pin <= BOARD_RESTRICTED_FLASH_END) {
        if (reason_out) *reason_out = "Internal SPI Flash (LOCKED)";
        return true;
    }

    /* Native USB D- / D+: GPIO 19, 20 */
    if (pin == BOARD_PIN_USB_DM || pin == BOARD_PIN_USB_DP) {
        if (reason_out) *reason_out = "Native USB PHY (LOCKED)";
        return true;
    }

    /* Silicon Input-Only Pin: GPIO 46 */
    if (pin == GPIO_NUM_46) {
        if (reason_out) *reason_out = "Input-Only Silicon (No Output)";
        return false; /* Restricted for output, but usable as input */
    }

    /* Strapping Pins: GPIO 0, 3, 45 */
    if (pin == GPIO_NUM_0 || pin == GPIO_NUM_3 || pin == GPIO_NUM_45) {
        if (reason_out) *reason_out = "Strapping Pin (Boot Caution)";
        return false; /* Usable with electrical caution */
    }

    if (reason_out) *reason_out = "None (Available)";
    return false;
}

bool board_pin_is_allocated(gpio_num_t pin, periph_id_t *owner_out) {
    if (!board_pin_is_valid(pin)) {
        if (owner_out) *owner_out = PERIPH_ID_NONE;
        return false;
    }

    BOARD_LOCK();
    bool allocated = s_pin_registry[pin].is_allocated;
    if (owner_out) {
        *owner_out = s_pin_registry[pin].owner;
    }
    BOARD_UNLOCK();

    return allocated;
}

const pin_allocation_t *board_get_pin_allocation(gpio_num_t pin) {
    if (!board_pin_is_valid(pin)) {
        return NULL;
    }
    return &s_pin_registry[pin];
}

/* ========================================================================= */
/* PIN REGISTRATION & ALLOCATION ENGINE                                      */
/* ========================================================================= */

esp_err_t board_check_conflict(gpio_num_t pin, periph_id_t requesting_owner) {
    if (!board_pin_is_valid(pin)) {
        ESP_LOGE(TAG, "Conflict check failed: GPIO%d is invalid", (int)pin);
        return ESP_ERR_INVALID_ARG;
    }

    const char *reason = NULL;
    if (board_pin_is_restricted(pin, &reason)) {
        /* Allow dedicated internal owners to claim their respective pins */
        if (requesting_owner == PERIPH_ID_OCTAL_PSRAM &&
            pin >= BOARD_RESTRICTED_OCTAL_PSRAM_START && pin <= BOARD_RESTRICTED_OCTAL_PSRAM_END) {
            return ESP_OK;
        }
        if (requesting_owner == PERIPH_ID_SPI_FLASH &&
            pin >= BOARD_RESTRICTED_FLASH_START && pin <= BOARD_RESTRICTED_FLASH_END) {
            return ESP_OK;
        }
        if (requesting_owner == PERIPH_ID_USB_NATIVE &&
            (pin == BOARD_PIN_USB_DM || pin == BOARD_PIN_USB_DP)) {
            return ESP_OK;
        }

        ESP_LOGE(TAG, "Conflict check rejected: GPIO%d is restricted (%s)", (int)pin, reason);
        return ESP_ERR_NOT_SUPPORTED;
    }

    BOARD_LOCK();
    if (s_pin_registry[pin].is_allocated) {
        if (s_pin_registry[pin].owner != requesting_owner) {
            /* Check if shared bus is explicitly allowed */
            if ((s_pin_registry[pin].flags & PIN_FLAG_SHARED_BUS)) {
                BOARD_UNLOCK();
                return ESP_OK;
            }
            ESP_LOGW(TAG, "Conflict detected: GPIO%d already owned by %s (requested by %s)",
                     (int)pin,
                     board_periph_id_to_name(s_pin_registry[pin].owner),
                     board_periph_id_to_name(requesting_owner));
            BOARD_UNLOCK();
            return ESP_ERR_INVALID_STATE;
        }
    }
    BOARD_UNLOCK();

    return ESP_OK;
}

esp_err_t board_register_pin(gpio_num_t pin, periph_id_t owner, const char *owner_name, pin_flags_t flags) {
    if (!board_pin_is_valid(pin)) {
        ESP_LOGE(TAG, "Cannot register invalid GPIO%d", (int)pin);
        return ESP_ERR_INVALID_ARG;
    }

    /* Reject output mode on input-only pin (GPIO 46) */
    if (pin == GPIO_NUM_46 && (flags & PIN_FLAG_OUTPUT)) {
        ESP_LOGE(TAG, "Cannot configure GPIO46 as OUTPUT (silicon input-only restriction)");
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_err_t err = board_check_conflict(pin, owner);
    if (err != ESP_OK) {
        return err;
    }

    BOARD_LOCK();
    pin_allocation_t *p = &s_pin_registry[pin];
    p->pin = pin;
    p->owner = owner;
    p->owner_name = owner_name ? owner_name : board_periph_id_to_name(owner);
    p->flags = flags;
    p->is_allocated = true;
    if (flags & PIN_FLAG_LOCKED) {
        p->is_locked = true;
    }

    /* Update peripheral pin mask */
    if (owner < PERIPH_ID_MAX) {
        s_periph_table[owner].pin_mask |= (1ULL << pin);
    }
    BOARD_UNLOCK();

    ESP_LOGD(TAG, "Registered GPIO%d -> Owner: %s, Flags: 0x%04X", (int)pin, p->owner_name, (unsigned)flags);
    return ESP_OK;
}

esp_err_t board_release_pin(gpio_num_t pin, periph_id_t owner) {
    if (!board_pin_is_valid(pin)) {
        return ESP_ERR_INVALID_ARG;
    }

    BOARD_LOCK();
    pin_allocation_t *p = &s_pin_registry[pin];
    if (!p->is_allocated) {
        BOARD_UNLOCK();
        return ESP_OK;
    }

    if (p->is_locked) {
        ESP_LOGE(TAG, "Cannot release locked pin GPIO%d (owned by %s)", (int)pin, p->owner_name);
        BOARD_UNLOCK();
        return ESP_ERR_INVALID_STATE;
    }

    if (p->owner != owner && owner != PERIPH_ID_SYSTEM) {
        ESP_LOGE(TAG, "Permission denied: GPIO%d owned by %s, release attempted by %s",
                 (int)pin, p->owner_name, board_periph_id_to_name(owner));
        BOARD_UNLOCK();
        return ESP_ERR_INVALID_STATE;
    }

    p->is_allocated = false;
    p->owner = PERIPH_ID_NONE;
    p->owner_name = "None";
    p->flags = PIN_FLAG_NONE;

    if (owner < PERIPH_ID_MAX) {
        s_periph_table[owner].pin_mask &= ~(1ULL << pin);
    }
    BOARD_UNLOCK();

    ESP_LOGD(TAG, "Released GPIO%d", (int)pin);
    return ESP_OK;
}

esp_err_t board_register_peripheral(periph_id_t id, const char *name, uint64_t pin_mask) {
    if (id >= PERIPH_ID_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    BOARD_LOCK();
    s_periph_table[id].id = id;
    s_periph_table[id].name = name ? name : board_periph_id_to_name(id);
    s_periph_table[id].pin_mask |= pin_mask;
    s_periph_table[id].presence = HW_PRESENCE_UNKNOWN;
    s_periph_table[id].last_error = ESP_OK;
    BOARD_UNLOCK();

    return ESP_OK;
}

hw_presence_t board_get_periph_presence(periph_id_t id) {
    if (id >= PERIPH_ID_MAX) {
        return HW_PRESENCE_UNKNOWN;
    }
    BOARD_LOCK();
    hw_presence_t st = s_periph_table[id].presence;
    BOARD_UNLOCK();
    return st;
}

void board_set_periph_presence(periph_id_t id, hw_presence_t presence, esp_err_t last_error) {
    if (id >= PERIPH_ID_MAX) {
        return;
    }
    BOARD_LOCK();
    s_periph_table[id].presence = presence;
    s_periph_table[id].last_error = last_error;
    BOARD_UNLOCK();

    ESP_LOGI(TAG, "Peripheral [%s] status: %s (err=0x%x)",
             board_periph_id_to_name(id),
             board_hw_presence_to_str(presence),
             (unsigned)last_error);
}

/* ========================================================================= */
/* HARDWARE PRESENCE DETECTION PROBES                                        */
/* ========================================================================= */

hw_presence_t board_check_nrf24_presence(void) {
    BOARD_LOCK();
    if (s_periph_table[PERIPH_ID_SPI2_NRF24].presence != HW_PRESENCE_UNKNOWN) {
        hw_presence_t st = s_periph_table[PERIPH_ID_SPI2_NRF24].presence;
        BOARD_UNLOCK();
        return st;
    }
    BOARD_UNLOCK();

    ESP_LOGI(TAG, "Probing NRF24L01+ on SPI2 (MOSI=11, MISO=13, SCK=12, CS=10, CE=14)...");

    /* Configure CSN pin as output */
    gpio_config_t cs_cfg = {
        .pin_bit_mask = (1ULL << BOARD_PIN_RF_SPI_CS),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cs_cfg);
    gpio_set_level(BOARD_PIN_RF_SPI_CS, 1);

    /* Configure CE pin as output */
    gpio_config_t ce_cfg = {
        .pin_bit_mask = (1ULL << BOARD_PIN_RF_CE),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&ce_cfg);
    gpio_set_level(BOARD_PIN_RF_CE, 0);

    /* Initialize SPI bus for NRF24 */
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = BOARD_PIN_RF_SPI_MOSI,
        .miso_io_num = BOARD_PIN_RF_SPI_MISO,
        .sclk_io_num = BOARD_PIN_RF_SPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };

    esp_err_t err = spi_bus_initialize(BOARD_RF_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    bool bus_init_by_us = (err == ESP_OK);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "NRF24 SPI bus initialization failed: %s", esp_err_to_name(err));
        board_set_periph_presence(PERIPH_ID_SPI2_NRF24, HW_PRESENCE_FAULT, err);
        return HW_PRESENCE_FAULT;
    }

    spi_device_interface_config_t dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .clock_speed_hz = BOARD_RF_SPI_CLOCK_HZ,
        .mode = 0,
        .spics_io_num = BOARD_PIN_RF_SPI_CS,
        .queue_size = 1,
    };

    spi_device_handle_t dev = NULL;
    err = spi_bus_add_device(BOARD_RF_SPI_HOST, &dev_cfg, &dev);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "NRF24 spi_bus_add_device failed: %s", esp_err_to_name(err));
        if (bus_init_by_us) spi_bus_free(BOARD_RF_SPI_HOST);
        board_set_periph_presence(PERIPH_ID_SPI2_NRF24, HW_PRESENCE_FAULT, err);
        return HW_PRESENCE_FAULT;
    }

    /* Probe CONFIG register (0x00). Default reset value is 0x08 */
    uint8_t tx[2] = { 0x00, 0xFF };
    uint8_t rx[2] = { 0x00, 0x00 };
    spi_transaction_t trans = {
        .length = 16,
        .tx_buffer = tx,
        .rx_buffer = rx,
    };

    err = spi_device_polling_transmit(dev, &trans);
    spi_bus_remove_device(dev);
    if (bus_init_by_us) {
        spi_bus_free(BOARD_RF_SPI_HOST);
    }

    hw_presence_t presence = HW_PRESENCE_DISCONNECTED;
    if (err == ESP_OK) {
        uint8_t config_val = rx[1];
        ESP_LOGD(TAG, "NRF24 probe returned CONFIG=0x%02X (STATUS=0x%02X)", config_val, rx[0]);
        /* Disconnected bus floats high (0xFF) or shorts low (0x00) */
        if (config_val != 0x00 && config_val != 0xFF) {
            presence = HW_PRESENCE_AVAILABLE;
            ESP_LOGI(TAG, "NRF24L01+ hardware verified present (CONFIG=0x%02X)", config_val);
        } else {
            ESP_LOGW(TAG, "NRF24L01+ disconnected or unpowered (CONFIG=0x%02X)", config_val);
        }
    } else {
        ESP_LOGW(TAG, "NRF24 SPI polling failed: %s", esp_err_to_name(err));
    }

    board_set_periph_presence(PERIPH_ID_SPI2_NRF24, presence, err);
    return presence;
}

hw_presence_t board_check_i2c_presence(uint8_t addr) {
    ESP_LOGD(TAG, "Probing I2C device at 0x%02X on SDA=8, SCL=9...", addr);

    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = BOARD_I2C_PORT,
        .scl_io_num = BOARD_PIN_I2C_SCL,
        .sda_io_num = BOARD_PIN_I2C_SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    esp_err_t err = i2c_new_master_bus(&i2c_bus_config, &bus_handle);
    bool bus_owned = (err == ESP_OK);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return HW_PRESENCE_FAULT;
    }

    if (bus_owned) {
        err = i2c_master_probe(bus_handle, addr, BOARD_I2C_TIMEOUT_MS);
        i2c_del_master_bus(bus_handle);
    } else {
        err = ESP_ERR_INVALID_STATE;
    }

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "I2C device detected at 0x%02X", addr);
        return HW_PRESENCE_AVAILABLE;
    }

    return HW_PRESENCE_DISCONNECTED;
}

hw_presence_t board_check_display_presence(void) {
    BOARD_LOCK();
    hw_presence_t st = s_periph_table[PERIPH_ID_SPI3_DISPLAY].presence;
    BOARD_UNLOCK();
    return st;
}

/* ========================================================================= */
/* DIAGNOSTIC TABLE DUMP                                                     */
/* ========================================================================= */

void board_dump_pinout_table(void) {
    ESP_LOGI(TAG, "=================================================================================");
    ESP_LOGI(TAG, "         %s PIN ALLOCATION & RESTRICTION REGISTRY", BOARD_TARGET_NAME);
    ESP_LOGI(TAG, "=================================================================================");
    ESP_LOGI(TAG, " GPIO | Location     | Owner Peripheral     | Direction / Flags        | Status   ");
    ESP_LOGI(TAG, "------+--------------+----------------------+--------------------------+---------");

    BOARD_LOCK();
    for (int pin = 0; pin < BOARD_TOTAL_GPIOS; pin++) {
        if (!board_pin_is_valid((gpio_num_t)pin)) {
            continue;
        }

        const pin_allocation_t *p = &s_pin_registry[pin];
        const char *loc = "Header";
        if (pin == 19 || pin == 20) loc = "USB-C Primary";
        else if (pin == 43 || pin == 44) loc = "USB-C UART";
        else if (pin == 48) loc = "Onboard RGB";
        else if (pin >= 27 && pin <= 32) loc = "Flash Module";
        else if (pin >= 33 && pin <= 38) loc = "PSRAM Module";
        else if (pin <= 20) loc = "Left Header";
        else loc = "Right Header";

        char flag_buf[32] = {0};
        if (p->flags & PIN_FLAG_PSRAM_BUS)     strcat(flag_buf, "PSRAM_BUS ");
        if (p->flags & PIN_FLAG_USB_DEDICATED) strcat(flag_buf, "USB_OTG ");
        if (p->flags & PIN_FLAG_INPUT_ONLY)    strcat(flag_buf, "INPUT_ONLY ");
        if (p->flags & PIN_FLAG_STRAPPING)     strcat(flag_buf, "STRAPPING ");
        if (p->flags & PIN_FLAG_LOCKED)        strcat(flag_buf, "LOCKED ");
        if (p->flags & PIN_FLAG_BIDIR)         strcat(flag_buf, "BI-DIR ");
        else if (p->flags & PIN_FLAG_OUTPUT)   strcat(flag_buf, "OUTPUT ");
        else if (p->flags & PIN_FLAG_INPUT)    strcat(flag_buf, "INPUT ");

        if (flag_buf[0] == '\0') {
            strcpy(flag_buf, "FREE / UNUSED");
        }

        ESP_LOGI(TAG, " %4d | %-12s | %-20s | %-24s | %s",
                 pin,
                 loc,
                 p->owner_name ? p->owner_name : "Free",
                 flag_buf,
                 p->is_locked ? "LOCKED" : (p->is_allocated ? "ACTIVE" : "FREE"));
    }
    BOARD_UNLOCK();
    ESP_LOGI(TAG, "=================================================================================");
}

/* ========================================================================= */
/* SYSTEM BOARD INITIALIZATION                                               */
/* ========================================================================= */

esp_err_t board_init(void) {
    if (s_board_initialized) {
        return ESP_OK;
    }

    if (!s_board_mutex) {
        s_board_mutex = xSemaphoreCreateRecursiveMutex();
    }

    BOARD_LOCK();
    memset(s_pin_registry, 0, sizeof(s_pin_registry));
    memset(s_periph_table, 0, sizeof(s_periph_table));

    /* Initialize all valid pins to clean default state */
    for (int i = 0; i < BOARD_TOTAL_GPIOS; i++) {
        s_pin_registry[i].pin = (gpio_num_t)i;
        s_pin_registry[i].name = "GPIO";
        s_pin_registry[i].owner = PERIPH_ID_NONE;
        s_pin_registry[i].owner_name = "Unassigned";
        s_pin_registry[i].flags = PIN_FLAG_NONE;
        s_pin_registry[i].is_allocated = false;
        s_pin_registry[i].is_locked = false;
    }

    /* --------------------------------------------------------------------- */
    /* 1. LOCK RESTRICTED SILICON BUSES PERMANENTLY                         */
    /* --------------------------------------------------------------------- */

    /* Lock Octal PSRAM Bus: GPIO 33-38 */
    for (int p = BOARD_RESTRICTED_OCTAL_PSRAM_START; p <= BOARD_RESTRICTED_OCTAL_PSRAM_END; p++) {
        s_pin_registry[p].owner = PERIPH_ID_OCTAL_PSRAM;
        s_pin_registry[p].owner_name = "Octal PSRAM (AP_3v3)";
        s_pin_registry[p].flags = PIN_FLAG_PSRAM_BUS | PIN_FLAG_LOCKED | PIN_FLAG_BIDIR;
        s_pin_registry[p].is_allocated = true;
        s_pin_registry[p].is_locked = true;
    }
    s_periph_table[PERIPH_ID_OCTAL_PSRAM].id = PERIPH_ID_OCTAL_PSRAM;
    s_periph_table[PERIPH_ID_OCTAL_PSRAM].name = "Octal PSRAM";
    s_periph_table[PERIPH_ID_OCTAL_PSRAM].presence = HW_PRESENCE_AVAILABLE;
    s_periph_table[PERIPH_ID_OCTAL_PSRAM].pin_mask = BOARD_OCTAL_PSRAM_PINS_MASK;

    /* Lock Internal SPI Flash: GPIO 27-32 */
    for (int p = BOARD_RESTRICTED_FLASH_START; p <= BOARD_RESTRICTED_FLASH_END; p++) {
        s_pin_registry[p].owner = PERIPH_ID_SPI_FLASH;
        s_pin_registry[p].owner_name = "16MB Flash (DIO)";
        s_pin_registry[p].flags = PIN_FLAG_LOCKED | PIN_FLAG_BIDIR;
        s_pin_registry[p].is_allocated = true;
        s_pin_registry[p].is_locked = true;
    }
    s_periph_table[PERIPH_ID_SPI_FLASH].id = PERIPH_ID_SPI_FLASH;
    s_periph_table[PERIPH_ID_SPI_FLASH].name = "16MB SPI Flash";
    s_periph_table[PERIPH_ID_SPI_FLASH].presence = HW_PRESENCE_AVAILABLE;

    /* Lock Native USB-OTG PHY: GPIO 19, 20 */
    s_pin_registry[BOARD_PIN_USB_DM].owner = PERIPH_ID_USB_NATIVE;
    s_pin_registry[BOARD_PIN_USB_DM].owner_name = "Native USB D-";
    s_pin_registry[BOARD_PIN_USB_DM].flags = PIN_FLAG_USB_DEDICATED | PIN_FLAG_LOCKED | PIN_FLAG_BIDIR;
    s_pin_registry[BOARD_PIN_USB_DM].is_allocated = true;
    s_pin_registry[BOARD_PIN_USB_DM].is_locked = true;

    s_pin_registry[BOARD_PIN_USB_DP].owner = PERIPH_ID_USB_NATIVE;
    s_pin_registry[BOARD_PIN_USB_DP].owner_name = "Native USB D+";
    s_pin_registry[BOARD_PIN_USB_DP].flags = PIN_FLAG_USB_DEDICATED | PIN_FLAG_LOCKED | PIN_FLAG_BIDIR;
    s_pin_registry[BOARD_PIN_USB_DP].is_allocated = true;
    s_pin_registry[BOARD_PIN_USB_DP].is_locked = true;

    s_periph_table[PERIPH_ID_USB_NATIVE].id = PERIPH_ID_USB_NATIVE;
    s_periph_table[PERIPH_ID_USB_NATIVE].name = "Native USB-OTG";
    s_periph_table[PERIPH_ID_USB_NATIVE].presence = HW_PRESENCE_AVAILABLE;
    s_periph_table[PERIPH_ID_USB_NATIVE].pin_mask = BOARD_NATIVE_USB_PINS_MASK;

    /* Flag Input-Only Silicon: GPIO 46 */
    s_pin_registry[GPIO_NUM_46].flags |= (PIN_FLAG_INPUT_ONLY | PIN_FLAG_STRAPPING);

    /* Flag Strapping Pins: GPIO 0, 3, 45 */
    s_pin_registry[GPIO_NUM_0].flags  |= PIN_FLAG_STRAPPING;
    s_pin_registry[GPIO_NUM_3].flags  |= PIN_FLAG_STRAPPING;
    s_pin_registry[GPIO_NUM_45].flags |= PIN_FLAG_STRAPPING;

    /* --------------------------------------------------------------------- */
    /* 2. REGISTER STATIC 7SEMI HARDWARE SUBSYSTEMS                         */
    /* --------------------------------------------------------------------- */

    /* Primary I2C (I2C0): GPIO 8 (SDA), GPIO 9 (SCL) */
    s_pin_registry[BOARD_PIN_I2C_SDA].owner = PERIPH_ID_I2C0;
    s_pin_registry[BOARD_PIN_I2C_SDA].owner_name = "Primary I2C SDA";
    s_pin_registry[BOARD_PIN_I2C_SDA].flags = PIN_FLAG_BIDIR | PIN_FLAG_OPEN_DRAIN | PIN_FLAG_PULLUP;
    s_pin_registry[BOARD_PIN_I2C_SDA].is_allocated = true;

    s_pin_registry[BOARD_PIN_I2C_SCL].owner = PERIPH_ID_I2C0;
    s_pin_registry[BOARD_PIN_I2C_SCL].owner_name = "Primary I2C SCL";
    s_pin_registry[BOARD_PIN_I2C_SCL].flags = PIN_FLAG_OUTPUT | PIN_FLAG_OPEN_DRAIN | PIN_FLAG_PULLUP;
    s_pin_registry[BOARD_PIN_I2C_SCL].is_allocated = true;

    s_periph_table[PERIPH_ID_I2C0].id = PERIPH_ID_I2C0;
    s_periph_table[PERIPH_ID_I2C0].name = "Primary I2C (I2C0)";
    s_periph_table[PERIPH_ID_I2C0].pin_mask = (1ULL << BOARD_PIN_I2C_SDA) | (1ULL << BOARD_PIN_I2C_SCL);
    s_periph_table[PERIPH_ID_I2C0].presence = HW_PRESENCE_AVAILABLE;

    /* Dedicated RF SPI (SPI2): NRF24L01+PA+LNA */
    s_pin_registry[BOARD_PIN_RF_SPI_CS].owner = PERIPH_ID_SPI2_NRF24;
    s_pin_registry[BOARD_PIN_RF_SPI_CS].owner_name = "NRF24 CSN";
    s_pin_registry[BOARD_PIN_RF_SPI_CS].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_RF_SPI_CS].is_allocated = true;

    s_pin_registry[BOARD_PIN_RF_SPI_MOSI].owner = PERIPH_ID_SPI2_NRF24;
    s_pin_registry[BOARD_PIN_RF_SPI_MOSI].owner_name = "NRF24 MOSI";
    s_pin_registry[BOARD_PIN_RF_SPI_MOSI].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_RF_SPI_MOSI].is_allocated = true;

    s_pin_registry[BOARD_PIN_RF_SPI_SCK].owner = PERIPH_ID_SPI2_NRF24;
    s_pin_registry[BOARD_PIN_RF_SPI_SCK].owner_name = "NRF24 SCK";
    s_pin_registry[BOARD_PIN_RF_SPI_SCK].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_RF_SPI_SCK].is_allocated = true;

    s_pin_registry[BOARD_PIN_RF_SPI_MISO].owner = PERIPH_ID_SPI2_NRF24;
    s_pin_registry[BOARD_PIN_RF_SPI_MISO].owner_name = "NRF24 MISO";
    s_pin_registry[BOARD_PIN_RF_SPI_MISO].flags = PIN_FLAG_INPUT;
    s_pin_registry[BOARD_PIN_RF_SPI_MISO].is_allocated = true;

    s_pin_registry[BOARD_PIN_RF_CE].owner = PERIPH_ID_SPI2_NRF24;
    s_pin_registry[BOARD_PIN_RF_CE].owner_name = "NRF24 CE";
    s_pin_registry[BOARD_PIN_RF_CE].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_RF_CE].is_allocated = true;

    s_periph_table[PERIPH_ID_SPI2_NRF24].id = PERIPH_ID_SPI2_NRF24;
    s_periph_table[PERIPH_ID_SPI2_NRF24].name = "NRF24L01+ RF (SPI2)";
    s_periph_table[PERIPH_ID_SPI2_NRF24].pin_mask = (1ULL << BOARD_PIN_RF_SPI_CS) |
                                                    (1ULL << BOARD_PIN_RF_SPI_MOSI) |
                                                    (1ULL << BOARD_PIN_RF_SPI_SCK) |
                                                    (1ULL << BOARD_PIN_RF_SPI_MISO) |
                                                    (1ULL << BOARD_PIN_RF_CE);
    s_periph_table[PERIPH_ID_SPI2_NRF24].presence = HW_PRESENCE_UNKNOWN;

    /* Dedicated Display SPI (SPI3): 1.8" ST7735S TFT */
    s_pin_registry[BOARD_PIN_DISP_CS].owner = PERIPH_ID_SPI3_DISPLAY;
    s_pin_registry[BOARD_PIN_DISP_CS].owner_name = "ST7735S CS";
    s_pin_registry[BOARD_PIN_DISP_CS].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_DISP_CS].is_allocated = true;

    s_pin_registry[BOARD_PIN_DISP_DC].owner = PERIPH_ID_SPI3_DISPLAY;
    s_pin_registry[BOARD_PIN_DISP_DC].owner_name = "ST7735S DC/A0";
    s_pin_registry[BOARD_PIN_DISP_DC].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_DISP_DC].is_allocated = true;

    s_pin_registry[BOARD_PIN_DISP_RST].owner = PERIPH_ID_SPI3_DISPLAY;
    s_pin_registry[BOARD_PIN_DISP_RST].owner_name = "ST7735S RST";
    s_pin_registry[BOARD_PIN_DISP_RST].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_DISP_RST].is_allocated = true;

    s_pin_registry[BOARD_PIN_DISP_MOSI].owner = PERIPH_ID_SPI3_DISPLAY;
    s_pin_registry[BOARD_PIN_DISP_MOSI].owner_name = "ST7735S MOSI";
    s_pin_registry[BOARD_PIN_DISP_MOSI].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_DISP_MOSI].is_allocated = true;

    s_pin_registry[BOARD_PIN_DISP_SCK].owner = PERIPH_ID_SPI3_DISPLAY;
    s_pin_registry[BOARD_PIN_DISP_SCK].owner_name = "ST7735S SCK";
    s_pin_registry[BOARD_PIN_DISP_SCK].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_DISP_SCK].is_allocated = true;

    s_periph_table[PERIPH_ID_SPI3_DISPLAY].id = PERIPH_ID_SPI3_DISPLAY;
    s_periph_table[PERIPH_ID_SPI3_DISPLAY].name = "ST7735S 128x160 TFT (SPI3)";
    s_periph_table[PERIPH_ID_SPI3_DISPLAY].pin_mask = (1ULL << BOARD_PIN_DISP_CS) |
                                                      (1ULL << BOARD_PIN_DISP_DC) |
                                                      (1ULL << BOARD_PIN_DISP_RST) |
                                                      (1ULL << BOARD_PIN_DISP_MOSI) |
                                                      (1ULL << BOARD_PIN_DISP_SCK);
    s_periph_table[PERIPH_ID_SPI3_DISPLAY].presence = HW_PRESENCE_AVAILABLE;

    /* UART1: GPIO 43 (TX), GPIO 44 (RX) */
    s_pin_registry[BOARD_PIN_UART1_TX].owner = PERIPH_ID_UART1;
    s_pin_registry[BOARD_PIN_UART1_TX].owner_name = "UART1 TX";
    s_pin_registry[BOARD_PIN_UART1_TX].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_UART1_TX].is_allocated = true;

    s_pin_registry[BOARD_PIN_UART1_RX].owner = PERIPH_ID_UART1;
    s_pin_registry[BOARD_PIN_UART1_RX].owner_name = "UART1 RX";
    s_pin_registry[BOARD_PIN_UART1_RX].flags = PIN_FLAG_INPUT;
    s_pin_registry[BOARD_PIN_UART1_RX].is_allocated = true;

    s_periph_table[PERIPH_ID_UART1].id = PERIPH_ID_UART1;
    s_periph_table[PERIPH_ID_UART1].name = "UART1 Telemetry";
    s_periph_table[PERIPH_ID_UART1].pin_mask = (1ULL << BOARD_PIN_UART1_TX) | (1ULL << BOARD_PIN_UART1_RX);
    s_periph_table[PERIPH_ID_UART1].presence = HW_PRESENCE_AVAILABLE;

    /* UART2: GPIO 17 (TX), GPIO 18 (RX) */
    s_pin_registry[BOARD_PIN_UART2_TX].owner = PERIPH_ID_UART2;
    s_pin_registry[BOARD_PIN_UART2_TX].owner_name = "UART2 TX";
    s_pin_registry[BOARD_PIN_UART2_TX].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_UART2_TX].is_allocated = true;

    s_pin_registry[BOARD_PIN_UART2_RX].owner = PERIPH_ID_UART2;
    s_pin_registry[BOARD_PIN_UART2_RX].owner_name = "UART2 RX";
    s_pin_registry[BOARD_PIN_UART2_RX].flags = PIN_FLAG_INPUT;
    s_pin_registry[BOARD_PIN_UART2_RX].is_allocated = true;

    s_periph_table[PERIPH_ID_UART2].id = PERIPH_ID_UART2;
    s_periph_table[PERIPH_ID_UART2].name = "UART2 Expansion";
    s_periph_table[PERIPH_ID_UART2].pin_mask = (1ULL << BOARD_PIN_UART2_TX) | (1ULL << BOARD_PIN_UART2_RX);
    s_periph_table[PERIPH_ID_UART2].presence = HW_PRESENCE_AVAILABLE;

    /* Onboard WS2812B NeoPixel RGB LED: GPIO 48 */
    s_pin_registry[BOARD_PIN_RGB_LED].owner = PERIPH_ID_RGB_NEOPIXEL;
    s_pin_registry[BOARD_PIN_RGB_LED].owner_name = "WS2812B RGB";
    s_pin_registry[BOARD_PIN_RGB_LED].flags = PIN_FLAG_OUTPUT;
    s_pin_registry[BOARD_PIN_RGB_LED].is_allocated = true;

    s_periph_table[PERIPH_ID_RGB_NEOPIXEL].id = PERIPH_ID_RGB_NEOPIXEL;
    s_periph_table[PERIPH_ID_RGB_NEOPIXEL].name = "WS2812B RGB LED";
    s_periph_table[PERIPH_ID_RGB_NEOPIXEL].pin_mask = (1ULL << BOARD_PIN_RGB_LED);
    s_periph_table[PERIPH_ID_RGB_NEOPIXEL].presence = HW_PRESENCE_AVAILABLE;

    s_board_initialized = true;
    BOARD_UNLOCK();

    ESP_LOGI(TAG, "Initialized %s HAL (God Mode V2 Authority)", BOARD_TARGET_NAME);
    return ESP_OK;
}
