#include "modules/deauth_monitor.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "core/serial_manager.h"
#include "core/glog.h"
#include "esp_wifi.h"

typedef void (*wifi_promiscuous_cb_t_t)(void *buf, wifi_promiscuous_pkt_type_t type);
extern void wifi_manager_start_monitor_mode(wifi_promiscuous_cb_t_t callback);
extern void wifi_manager_stop_monitor_mode(void);
extern void wifi_deauth_scan_callback(void *buf, wifi_promiscuous_pkt_type_t type);
#include <string.h>
#include <stdio.h>

static const char *TAG = "DeauthMonitor";

static deauth_event_t s_history[DEAUTH_HISTORY_MAX];
static uint32_t s_history_head = 0;
static uint32_t s_history_count = 0;
static uint32_t s_total_deauths = 0;
static uint32_t s_total_disassocs = 0;
static bool s_active = false;
static SemaphoreHandle_t s_mutex = NULL;

/* Sliding rate window */
static uint32_t s_window_start_ms = 0;
static uint32_t s_window_frames = 0;
static uint32_t s_current_rate = 0;
static uint32_t s_last_alert_ms = 0;

static uint32_t get_time_ms(void) {
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

static const char *reason_to_str(uint16_t code) {
    switch (code) {
        case 1:  return "Unspecified";
        case 2:  return "Previous auth no longer valid";
        case 3:  return "Deauth: Station leaving BSS";
        case 4:  return "Disassoc: Inactivity";
        case 6:  return "Class 2 frame from nonauth STA";
        case 7:  return "Class 3 frame from nonassociated STA";
        case 8:  return "Disassoc: Station leaving BSS";
        case 15: return "4-Way Handshake timeout";
        case 23: return "IEEE 802.1X auth failed";
        default: return "Reserved / Proprietary";
    }
}

esp_err_t deauth_monitor_init(void) {
    if (!s_mutex) {
        s_mutex = xSemaphoreCreateMutex();
    }
    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        memset(s_history, 0, sizeof(s_history));
        s_history_head = 0;
        s_history_count = 0;
        s_total_deauths = 0;
        s_total_disassocs = 0;
        s_current_rate = 0;
        s_active = false;
        xSemaphoreGive(s_mutex);
    }
    ESP_LOGI(TAG, "Passive Deauthentication Monitor initialized");
    return ESP_OK;
}

esp_err_t deauth_monitor_start(void) {
    if (!s_mutex) deauth_monitor_init();
    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_active = true;
        s_window_start_ms = get_time_ms();
        s_window_frames = 0;
        xSemaphoreGive(s_mutex);
        wifi_manager_start_monitor_mode(wifi_deauth_scan_callback);
        ESP_LOGI(TAG, "Passive deauth monitor started with promiscuous capture");
        return ESP_OK;
    }
    return ESP_ERR_TIMEOUT;
}

esp_err_t deauth_monitor_stop(void) {
    if (s_mutex && xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_active = false;
        xSemaphoreGive(s_mutex);
        wifi_manager_stop_monitor_mode();
        ESP_LOGI(TAG, "Passive deauth monitor stopped");
        return ESP_OK;
    }
    return ESP_OK;
}

bool deauth_monitor_is_active(void) {
    return s_active;
}

void deauth_monitor_clear(void) {
    if (s_mutex && xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        memset(s_history, 0, sizeof(s_history));
        s_history_head = 0;
        s_history_count = 0;
        s_total_deauths = 0;
        s_total_disassocs = 0;
        s_current_rate = 0;
        xSemaphoreGive(s_mutex);
    }
}

uint32_t deauth_monitor_get_total_count(void) {
    return s_total_deauths + s_total_disassocs;
}

uint32_t deauth_monitor_get_rate(void) {
    if (s_current_rate > 0 && (get_time_ms() - s_window_start_ms) >= 2000U) {
        return 0;
    }
    return s_current_rate;
}

void deauth_monitor_process_frame(const uint8_t *frame, uint16_t length, int8_t rssi) {
    (void)rssi;
    if (!s_active || !frame || length < 26) return;

    /* Check for 802.11 Management Frame (Type 0x00) */
    uint8_t frame_ctrl = frame[0];
    if ((frame_ctrl & 0x0C) != 0x00) {
        return; /* Not a management frame */
    }

    uint8_t subtype = (frame_ctrl >> 4) & 0x0F;
    if (subtype != 0x0C && subtype != 0x0A) {
        return; /* 0x0C = Deauth, 0x0A = Disassoc */
    }

    uint32_t now = get_time_ms();

    if (xSemaphoreTake(s_mutex, 0) != pdTRUE) {
        return; /* Drop if mutex locked to avoid blocking RX callback */
    }

    /* Compute sliding rate (1-second window) */
    s_window_frames++;
    if (now - s_window_start_ms >= 1000) {
        s_current_rate = s_window_frames;
        s_window_frames = 0;
        s_window_start_ms = now;
    }

    /* Extract MAC headers */
    const uint8_t *receiver    = &frame[4];
    const uint8_t *transmitter = &frame[10];
    const uint8_t *bssid       = &frame[16];
    uint16_t reason            = frame[24] | (frame[25] << 8);

    if (subtype == 0x0C) s_total_deauths++;
    else s_total_disassocs++;

    /* Record event in ring buffer */
    deauth_event_t *ev = &s_history[s_history_head];
    memcpy(ev->receiver, receiver, 6);
    memcpy(ev->transmitter, transmitter, 6);
    memcpy(ev->bssid, bssid, 6);
    ev->reason_code = reason;
    ev->frame_type = subtype;
    ev->timestamp_ms = now;

    s_history_head = (s_history_head + 1) % DEAUTH_HISTORY_MAX;
    if (s_history_count < DEAUTH_HISTORY_MAX) s_history_count++;

    bool emit_alert = false;
    uint32_t alert_rate = 0;
    uint8_t tx_snap[6] = {0}, rx_snap[6] = {0};
    if (s_current_rate >= DEAUTH_ALERT_THRESHOLD && (now - s_last_alert_ms > 2000)) {
        s_last_alert_ms = now;
        emit_alert = true;
        alert_rate = s_current_rate;
        memcpy(tx_snap, transmitter, 6);
        memcpy(rx_snap, receiver, 6);
    }

    xSemaphoreGive(s_mutex);

    if (emit_alert) {
        glog("\n[!] DEAUTH STORM DETECTED: %lu frames/sec!\n    Transmitter: %02X:%02X:%02X:%02X:%02X:%02X -> Target: %02X:%02X:%02X:%02X:%02X:%02X (Reason %d: %s)\n",
             (unsigned long)alert_rate,
             tx_snap[0], tx_snap[1], tx_snap[2], tx_snap[3], tx_snap[4], tx_snap[5],
             rx_snap[0], rx_snap[1], rx_snap[2], rx_snap[3], rx_snap[4], rx_snap[5],
             reason, reason_to_str(reason));
    }
}

void deauth_monitor_dump_status(void) {
    if (!s_mutex) deauth_monitor_init();

    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(500)) != pdTRUE) {
        glog("Deauth monitor busy.\n");
        return;
    }
    if (s_current_rate > 0 && (get_time_ms() - s_window_start_ms) >= 2000U) {
        s_current_rate = 0;
    }

    glog("\n=================================================================================\n");
    glog("             GHOSTESP PASSIVE DEAUTHENTICATION / DISASSOC MONITOR                \n");
    glog("=================================================================================\n");
    glog(" Status: %s | Total Deauths: %lu | Disassocs: %lu | Live Rate: %lu fps\n",
         s_active ? "ACTIVE" : "IDLE",
         (unsigned long)s_total_deauths, (unsigned long)s_total_disassocs, (unsigned long)s_current_rate);
    glog("---------------------------------------------------------------------------------\n");
    glog(" Time (s) | Type     | Transmitter MAC   | Receiver MAC      | Reason Code       \n");
    glog("----------+----------+-------------------+-------------------+-------------------\n");

    uint32_t now = get_time_ms();
    for (uint32_t i = 0; i < s_history_count; i++) {
        uint32_t idx = (s_history_head + DEAUTH_HISTORY_MAX - 1 - i) % DEAUTH_HISTORY_MAX;
        const deauth_event_t *ev = &s_history[idx];
        uint32_t age_s = (now >= ev->timestamp_ms) ? (now - ev->timestamp_ms) / 1000 : 0;

        glog(" %6lus | %-8s | %02X:%02X:%02X:%02X:%02X:%02X | %02X:%02X:%02X:%02X:%02X:%02X | %2d (%s)\n",
             (unsigned long)age_s,
             ev->frame_type == 0x0C ? "DEAUTH" : "DISASSOC",
             ev->transmitter[0], ev->transmitter[1], ev->transmitter[2], ev->transmitter[3], ev->transmitter[4], ev->transmitter[5],
             ev->receiver[0], ev->receiver[1], ev->receiver[2], ev->receiver[3], ev->receiver[4], ev->receiver[5],
             ev->reason_code, reason_to_str(ev->reason_code));
    }
    glog("=================================================================================\n\n");
    xSemaphoreGive(s_mutex);
}
