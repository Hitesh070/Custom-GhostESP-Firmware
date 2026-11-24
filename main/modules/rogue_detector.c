#include "modules/rogue_detector.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "core/serial_manager.h"
#include "core/glog.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "RogueDetector";

static rogue_record_t s_records[ROGUE_MAX_CANDIDATES];
static uint32_t s_record_count = 0;
static uint32_t s_threat_count = 0;
static bool s_active = false;
static SemaphoreHandle_t s_mutex = NULL;
static uint32_t s_last_alert_time_ms = 0;

static const char *auth_mode_to_str(uint8_t authmode) {
    switch (authmode) {
        case 0: return "OPEN";
        case 1: return "WEP";
        case 2: return "WPA_PSK";
        case 3: return "WPA2_PSK";
        case 4: return "WPA_WPA2_PSK";
        case 5: return "WPA2_ENTERPRISE";
        case 6: return "WPA3_PSK";
        case 7: return "WPA2_WPA3_PSK";
        case 8: return "WAPI_PSK";
        case 9: return "OWE";
        default: return "UNKNOWN";
    }
}

static uint32_t get_time_ms(void) {
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

esp_err_t rogue_detector_init(void) {
    if (!s_mutex) {
        s_mutex = xSemaphoreCreateMutex();
    }
    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        memset(s_records, 0, sizeof(s_records));
        s_record_count = 0;
        s_threat_count = 0;
        s_active = false;
        xSemaphoreGive(s_mutex);
    }
    ESP_LOGI(TAG, "Rogue AP & Evil Twin Detection Engine initialized");
    return ESP_OK;
}

esp_err_t rogue_detector_start(void) {
    if (!s_mutex) rogue_detector_init();
    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_active = true;
        xSemaphoreGive(s_mutex);
        ESP_LOGI(TAG, "Passive rogue detection started");
        return ESP_OK;
    }
    return ESP_ERR_TIMEOUT;
}

esp_err_t rogue_detector_stop(void) {
    if (s_mutex && xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_active = false;
        xSemaphoreGive(s_mutex);
        ESP_LOGI(TAG, "Passive rogue detection stopped");
        return ESP_OK;
    }
    return ESP_OK;
}

bool rogue_detector_is_active(void) {
    return s_active;
}

void rogue_detector_clear(void) {
    if (s_mutex && xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        memset(s_records, 0, sizeof(s_records));
        s_record_count = 0;
        s_threat_count = 0;
        xSemaphoreGive(s_mutex);
        ESP_LOGI(TAG, "Candidate database cleared");
    }
}

uint32_t rogue_detector_get_threat_count(void) {
    return s_threat_count;
}

void rogue_detector_process_ap(const uint8_t *bssid, const char *ssid, uint8_t channel, int8_t rssi, uint8_t authmode) {
    if (!s_active || !bssid || !ssid) return;
    if (ssid[0] == '\0') return; /* Ignore hidden / wildcard SSIDs for Evil Twin matching */

    uint32_t now = get_time_ms();

    if (xSemaphoreTake(s_mutex, 0) != pdTRUE) {
        return; /* Drop frame if mutex contended to avoid stalling Wi-Fi RX path */
    }

    /* 1. Check for known BSSID */
    for (uint32_t i = 0; i < s_record_count; i++) {
        if (memcmp(s_records[i].bssid, bssid, 6) == 0) {
            /* Known BSSID: check for security downgrade */
            if (s_records[i].authmode > 0 && authmode == 0) {
                s_records[i].threat_type = ROGUE_TYPE_SECURITY_DOWNGRADE;
                snprintf(s_records[i].evidence, sizeof(s_records[i].evidence),
                         "Security Downgrade: %s -> OPEN", auth_mode_to_str(s_records[i].authmode));
                s_threat_count++;
            }
            /* Check for unexplained channel jump */
            if (s_records[i].channel != channel) {
                s_records[i].threat_type = ROGUE_TYPE_CHANNEL_ANOMALY;
                snprintf(s_records[i].evidence, sizeof(s_records[i].evidence),
                         "Channel Jump: Ch%d -> Ch%d", s_records[i].channel, channel);
                s_records[i].channel = channel;
            }
            s_records[i].rssi = rssi;
            s_records[i].last_seen_ms = now;
            xSemaphoreGive(s_mutex);
            return;
        }
    }

    /* 2. Check for duplicate SSID with different BSSID (Evil Twin) */
    for (uint32_t i = 0; i < s_record_count; i++) {
        if (strncmp(s_records[i].ssid, ssid, ROGUE_SSID_MAX_LEN) == 0) {
            /* Same SSID from different BSSID */
            char orig_bssid[18], new_bssid[18];
            snprintf(orig_bssid, sizeof(orig_bssid), "%02X:%02X:%02X:%02X:%02X:%02X",
                     s_records[i].bssid[0], s_records[i].bssid[1], s_records[i].bssid[2],
                     s_records[i].bssid[3], s_records[i].bssid[4], s_records[i].bssid[5]);
            snprintf(new_bssid, sizeof(new_bssid), "%02X:%02X:%02X:%02X:%02X:%02X",
                     bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

            /* Insert new rogue record */
            if (s_record_count < ROGUE_MAX_CANDIDATES) {
                rogue_record_t *r = &s_records[s_record_count++];
                memcpy(r->bssid, bssid, 6);
                strncpy(r->ssid, ssid, sizeof(r->ssid) - 1);
                r->channel = channel;
                r->rssi = rssi;
                r->authmode = authmode;
                r->threat_type = ROGUE_TYPE_EVIL_TWIN;
                r->first_seen_ms = now;
                r->last_seen_ms = now;

                if (memcmp(s_records[i].bssid, bssid, 3) != 0) {
                    snprintf(r->evidence, sizeof(r->evidence), "Evil Twin (OUI Mismatch vs %s)", orig_bssid);
                } else {
                    snprintf(r->evidence, sizeof(r->evidence), "Evil Twin (Clone of %s)", orig_bssid);
                }
                s_threat_count++;

                /* Rate-limited serial alert (max 1 alert per 2 seconds) */
                if (now - s_last_alert_time_ms > 2000) {
                    s_last_alert_time_ms = now;
                    glog("\n[!] ROGUE ALERT: Evil Twin detected for '%s'!\n    Primary: %s (Ch%d, %d dBm)\n    Rogue:   %s (Ch%d, %d dBm)\n",
                         ssid, orig_bssid, s_records[i].channel, s_records[i].rssi,
                         new_bssid, channel, rssi);
                }
            }
            xSemaphoreGive(s_mutex);
            return;
        }
    }

    /* 3. New legitimate AP baseline candidate */
    if (s_record_count < ROGUE_MAX_CANDIDATES) {
        rogue_record_t *r = &s_records[s_record_count++];
        memcpy(r->bssid, bssid, 6);
        strncpy(r->ssid, ssid, sizeof(r->ssid) - 1);
        r->channel = channel;
        r->rssi = rssi;
        r->authmode = authmode;
        r->threat_type = ROGUE_TYPE_NONE;
        r->first_seen_ms = now;
        r->last_seen_ms = now;
        r->evidence[0] = '\0';
    }
    xSemaphoreGive(s_mutex);
}

void rogue_detector_dump_status(void) {
    if (!s_mutex) rogue_detector_init();

    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(500)) != pdTRUE) {
        glog("Rogue detector busy.\n");
        return;
    }

    glog("\n=================================================================================\n");
    glog("                   GHOSTESP ROGUE AP & EVIL TWIN RADAR REPORT                    \n");
    glog("=================================================================================\n");
    glog(" Status: %s | Tracked APs: %lu | Threats Detected: %lu\n",
         s_active ? "ACTIVE (Scanning)" : "IDLE", (unsigned long)s_record_count, (unsigned long)s_threat_count);
    glog("---------------------------------------------------------------------------------\n");
    glog(" BSSID             | Ch | RSSI | Security       | SSID                 | Threat  \n");
    glog("-------------------+----+------+----------------+----------------------+---------\n");

    for (uint32_t i = 0; i < s_record_count; i++) {
        const rogue_record_t *r = &s_records[i];
        const char *threat_str = "CLEAN";
        if (r->threat_type == ROGUE_TYPE_EVIL_TWIN) threat_str = "EVIL TWIN";
        else if (r->threat_type == ROGUE_TYPE_SECURITY_DOWNGRADE) threat_str = "DOWNGRADE";
        else if (r->threat_type == ROGUE_TYPE_CHANNEL_ANOMALY) threat_str = "CH ANOMALY";
        else if (r->threat_type == ROGUE_TYPE_VENDOR_MISMATCH) threat_str = "OUI MISMATCH";

        glog(" %02X:%02X:%02X:%02X:%02X:%02X | %2d | %4d | %-14s | %-20s | %s\n",
             r->bssid[0], r->bssid[1], r->bssid[2], r->bssid[3], r->bssid[4], r->bssid[5],
             r->channel, r->rssi, auth_mode_to_str(r->authmode),
             r->ssid, threat_str);

        if (r->threat_type != ROGUE_TYPE_NONE && r->evidence[0] != '\0') {
            glog("   └─> Evidence: %s\n", r->evidence);
        }
    }
    glog("=================================================================================\n\n");
    xSemaphoreGive(s_mutex);
}
