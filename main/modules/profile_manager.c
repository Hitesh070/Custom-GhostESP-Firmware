#include "modules/profile_manager.h"
#include "modules/rogue_detector.h"
#include "modules/deauth_monitor.h"
#include "esp_log.h"
#include "core/serial_manager.h"
#include "core/glog.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "ProfileManager";

static const profile_config_t s_preset_profiles[PROFILE_ID_MAX] = {
    [PROFILE_ID_COMBINED] = {
        .id = PROFILE_ID_COMBINED,
        .name = "Combined",
        .description = "Balanced multi-protocol wireless security assessment",
        .wifi_monitor_enabled = true,
        .rogue_detection_enabled = true,
        .deauth_monitor_enabled = true,
        .ble_scan_enabled = true,
        .nrf24_sweep_enabled = false,
        .ir_rx_enabled = true,
        .display_brightness_pct = 90,
        .log_verbosity = 2,
    },
    [PROFILE_ID_WIFI_RESEARCH] = {
        .id = PROFILE_ID_WIFI_RESEARCH,
        .name = "WiFi-Research",
        .description = "Dedicated 802.11 monitor, rogue AP detection & deauth storm tracking",
        .wifi_monitor_enabled = true,
        .rogue_detection_enabled = true,
        .deauth_monitor_enabled = true,
        .ble_scan_enabled = false,
        .nrf24_sweep_enabled = false,
        .ir_rx_enabled = false,
        .display_brightness_pct = 100,
        .log_verbosity = 3,
    },
    [PROFILE_ID_BLE_RESEARCH] = {
        .id = PROFILE_ID_BLE_RESEARCH,
        .name = "BLE-Research",
        .description = "High-duty Bluetooth Low Energy scanning, GATT analysis & radar",
        .wifi_monitor_enabled = false,
        .rogue_detection_enabled = false,
        .deauth_monitor_enabled = false,
        .ble_scan_enabled = true,
        .nrf24_sweep_enabled = false,
        .ir_rx_enabled = false,
        .display_brightness_pct = 80,
        .log_verbosity = 2,
    },
    [PROFILE_ID_NRF24_LAB] = {
        .id = PROFILE_ID_NRF24_LAB,
        .name = "NRF24-Lab",
        .description = "Dedicated SPI2 2.4GHz RF spectrum analysis & register diagnostics",
        .wifi_monitor_enabled = false,
        .rogue_detection_enabled = false,
        .deauth_monitor_enabled = false,
        .ble_scan_enabled = false,
        .nrf24_sweep_enabled = true,
        .ir_rx_enabled = false,
        .display_brightness_pct = 90,
        .log_verbosity = 2,
    },
    [PROFILE_ID_IR_LAB] = {
        .id = PROFILE_ID_IR_LAB,
        .name = "IR-Lab",
        .description = "Universal infrared protocol analyzer, timing decoder & blaster testing",
        .wifi_monitor_enabled = false,
        .rogue_detection_enabled = false,
        .deauth_monitor_enabled = false,
        .ble_scan_enabled = false,
        .nrf24_sweep_enabled = false,
        .ir_rx_enabled = true,
        .display_brightness_pct = 80,
        .log_verbosity = 2,
    },
    [PROFILE_ID_USB_LAB] = {
        .id = PROFILE_ID_USB_LAB,
        .name = "USB-Lab",
        .description = "Native USB-OTG HID test laboratory with explicit user verification",
        .wifi_monitor_enabled = false,
        .rogue_detection_enabled = false,
        .deauth_monitor_enabled = false,
        .ble_scan_enabled = false,
        .nrf24_sweep_enabled = false,
        .ir_rx_enabled = false,
        .display_brightness_pct = 85,
        .log_verbosity = 2,
    },
    [PROFILE_ID_DIAGNOSTICS] = {
        .id = PROFILE_ID_DIAGNOSTICS,
        .name = "Diagnostics",
        .description = "Full system hardware self-test, bus integrity & memory audit",
        .wifi_monitor_enabled = false,
        .rogue_detection_enabled = false,
        .deauth_monitor_enabled = false,
        .ble_scan_enabled = false,
        .nrf24_sweep_enabled = false,
        .ir_rx_enabled = false,
        .display_brightness_pct = 100,
        .log_verbosity = 4,
    },
    [PROFILE_ID_CUSTOM] = {
        .id = PROFILE_ID_CUSTOM,
        .name = "Custom",
        .description = "User-customized research configuration preset",
        .wifi_monitor_enabled = true,
        .rogue_detection_enabled = true,
        .deauth_monitor_enabled = true,
        .ble_scan_enabled = true,
        .nrf24_sweep_enabled = true,
        .ir_rx_enabled = true,
        .display_brightness_pct = 100,
        .log_verbosity = 2,
    }
};

static profile_id_t s_active_profile = PROFILE_ID_COMBINED;

esp_err_t profile_manager_init(void) {
    s_active_profile = PROFILE_ID_COMBINED;
    ESP_LOGI(TAG, "Profile Manager initialized (Default: Combined)");
    return ESP_OK;
}

esp_err_t profile_manager_apply(profile_id_t id) {
    if (id >= PROFILE_ID_MAX) return ESP_ERR_INVALID_ARG;

    s_active_profile = id;
    const profile_config_t *cfg = &s_preset_profiles[id];

    /* Dynamic subsystem orchestration */
    if (cfg->rogue_detection_enabled) {
        rogue_detector_start();
    } else {
        rogue_detector_stop();
    }

    if (cfg->deauth_monitor_enabled) {
        deauth_monitor_start();
    } else {
        deauth_monitor_stop();
    }

    ESP_LOGI(TAG, "Applied research profile: [%s] - %s", cfg->name, cfg->description);
    glog("\n[+] Activated Research Profile: [%s]\n    Description: %s\n", cfg->name, cfg->description);
    return ESP_OK;
}

const profile_config_t *profile_manager_get_current(void) {
    return &s_preset_profiles[s_active_profile];
}

void profile_manager_dump(void) {
    glog("\n=================================================================================\n");
    glog("                   GHOSTESP RESEARCH CONFIGURATION PROFILES                      \n");
    glog("=================================================================================\n");
    glog(" ID | Profile Name    | Active | Description\n");
    glog("----+-----------------+--------+-------------------------------------------------\n");

    for (int i = 0; i < PROFILE_ID_MAX; i++) {
        const profile_config_t *p = &s_preset_profiles[i];
        bool is_cur = (p->id == s_active_profile);
        glog(" %2d | %-15s | %-6s | %s\n",
             p->id, p->name, is_cur ? "YES *" : "no", p->description);
    }
    glog("=================================================================================\n");
    glog(" Usage: profile apply <id>   (e.g. 'profile apply 1' for WiFi-Research)\n\n");
}
