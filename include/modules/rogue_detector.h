#ifndef MODULES_ROGUE_DETECTOR_H
#define MODULES_ROGUE_DETECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#define ROGUE_MAX_CANDIDATES       64
#define ROGUE_REASON_MAX_LEN       64
#define ROGUE_SSID_MAX_LEN         33

typedef enum {
    ROGUE_TYPE_NONE = 0,
    ROGUE_TYPE_EVIL_TWIN,        /* Same SSID, different BSSID */
    ROGUE_TYPE_SECURITY_DOWNGRADE,/* Downgraded from WPA2/3 to Open/WEP */
    ROGUE_TYPE_CHANNEL_ANOMALY,  /* Unexpected channel change without CSA */
    ROGUE_TYPE_VENDOR_MISMATCH,  /* Divergent OUI vendor for identical SSID */
    ROGUE_TYPE_RSSI_SPIKE        /* Unnatural localized power surge */
} rogue_threat_type_t;

typedef struct {
    uint8_t             bssid[6];
    char                ssid[ROGUE_SSID_MAX_LEN];
    uint8_t             channel;
    int8_t              rssi;
    uint8_t             authmode;
    rogue_threat_type_t threat_type;
    uint32_t            first_seen_ms;
    uint32_t            last_seen_ms;
    char                evidence[ROGUE_REASON_MAX_LEN];
} rogue_record_t;

/**
 * @brief Initialize the Rogue AP & Evil Twin Detection Engine.
 */
esp_err_t rogue_detector_init(void);

/**
 * @brief Start passive rogue AP detection.
 */
esp_err_t rogue_detector_start(void);

/**
 * @brief Stop passive rogue AP detection.
 */
esp_err_t rogue_detector_stop(void);

/**
 * @brief Check if rogue detector is actively monitoring.
 */
bool rogue_detector_is_active(void);

/**
 * @brief Ingest an observed 802.11 beacon/probe frame for multi-vector threat correlation.
 *
 * @param bssid 6-byte AP MAC address
 * @param ssid Null-terminated SSID string
 * @param channel Radio channel (1-14)
 * @param rssi Received signal strength indicator
 * @param authmode Wi-Fi authentication mode (WIFI_AUTH_OPEN, WPA2_PSK, etc.)
 */
void rogue_detector_process_ap(const uint8_t *bssid, const char *ssid, uint8_t channel, int8_t rssi, uint8_t authmode);

/**
 * @brief Dump rogue detection report and candidate table to serial console.
 */
void rogue_detector_dump_status(void);

/**
 * @brief Clear all cached AP entries and detected alerts.
 */
void rogue_detector_clear(void);

/**
 * @brief Get total count of confirmed suspicious candidates.
 */
uint32_t rogue_detector_get_threat_count(void);

#ifdef __cplusplus
}
#endif

#endif /* MODULES_ROGUE_DETECTOR_H */
