#ifndef MODULES_DEAUTH_MONITOR_H
#define MODULES_DEAUTH_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#define DEAUTH_HISTORY_MAX       32
#define DEAUTH_ALERT_THRESHOLD    5 /* Alert if >= 5 deauths per second */

typedef struct {
    uint8_t  transmitter[6];
    uint8_t  receiver[6];
    uint8_t  bssid[6];
    uint16_t reason_code;
    uint8_t  frame_type;  /* 0x0C = Deauth, 0x0A = Disassoc */
    uint32_t timestamp_ms;
} deauth_event_t;

/**
 * @brief Initialize the Passive Deauthentication Activity Monitor.
 */
esp_err_t deauth_monitor_init(void);

/**
 * @brief Start passive deauthentication monitoring.
 */
esp_err_t deauth_monitor_start(void);

/**
 * @brief Stop passive deauthentication monitoring.
 */
esp_err_t deauth_monitor_stop(void);

/**
 * @brief Check if deauth monitor is actively capturing.
 */
bool deauth_monitor_is_active(void);

/**
 * @brief Ingest a raw 802.11 frame from promiscuous callback for deauth analysis.
 *
 * @param frame Pointer to raw 802.11 payload
 * @param length Total length of captured frame
 * @param rssi Received signal strength
 */
void deauth_monitor_process_frame(const uint8_t *frame, uint16_t length, int8_t rssi);

/**
 * @brief Dump deauth detection statistics and recent event log.
 */
void deauth_monitor_dump_status(void);

/**
 * @brief Reset statistics and event buffer.
 */
void deauth_monitor_clear(void);

/**
 * @brief Get total deauth frame count observed.
 */
uint32_t deauth_monitor_get_total_count(void);

/**
 * @brief Get current deauth frame rate (frames/sec).
 */
uint32_t deauth_monitor_get_rate(void);

#ifdef __cplusplus
}
#endif

#endif /* MODULES_DEAUTH_MONITOR_H */
