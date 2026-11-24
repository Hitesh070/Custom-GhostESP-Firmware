#ifndef MODULES_PROFILE_MANAGER_H
#define MODULES_PROFILE_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PROFILE_ID_COMBINED = 0,
    PROFILE_ID_WIFI_RESEARCH,
    PROFILE_ID_BLE_RESEARCH,
    PROFILE_ID_NRF24_LAB,
    PROFILE_ID_IR_LAB,
    PROFILE_ID_USB_LAB,
    PROFILE_ID_DIAGNOSTICS,
    PROFILE_ID_CUSTOM,
    PROFILE_ID_MAX
} profile_id_t;

typedef struct {
    profile_id_t id;
    const char  *name;
    const char  *description;
    bool         wifi_monitor_enabled;
    bool         rogue_detection_enabled;
    bool         deauth_monitor_enabled;
    bool         ble_scan_enabled;
    bool         nrf24_sweep_enabled;
    bool         ir_rx_enabled;
    uint8_t      display_brightness_pct;
    uint16_t     log_verbosity;
} profile_config_t;

/**
 * @brief Initialize the Configuration Profile Engine.
 */
esp_err_t profile_manager_init(void);

/**
 * @brief Activate a preset research configuration profile.
 *
 * @param id Profile ID to apply.
 * @return ESP_OK on success.
 */
esp_err_t profile_manager_apply(profile_id_t id);

/**
 * @brief Get currently active profile configuration.
 */
const profile_config_t *profile_manager_get_current(void);

/**
 * @brief Dump all available profiles and active status to serial console.
 */
void profile_manager_dump(void);

#ifdef __cplusplus
}
#endif

#endif /* MODULES_PROFILE_MANAGER_H */
