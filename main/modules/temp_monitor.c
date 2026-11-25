#include "modules/temp_monitor.h"
#include "core/glog.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#if SOC_TEMP_SENSOR_SUPPORTED
#include "driver/temperature_sensor.h"
#endif

static const char *TAG = "TempMonitor";

static SemaphoreHandle_t s_mutex = NULL;
static esp_timer_handle_t s_sample_timer = NULL;
#if SOC_TEMP_SENSOR_SUPPORTED
static temperature_sensor_handle_t s_tsens = NULL;
#endif

static temp_sample_t s_history[TEMP_MONITOR_HISTORY_MAX];
static size_t s_hist_head = 0;
static size_t s_hist_count = 0;

static temp_monitor_status_t s_status = {
    .initialized = false,
    .enabled = false,
    .valid = false,
    .current_c = 0.0f,
    .min_c = 999.0f,
    .max_c = -999.0f,
    .avg_c = 0.0f,
    .unit = TEMP_UNIT_CELSIUS,
    .interval_ms = TEMP_MONITOR_DEFAULT_INTERVAL_MS,
    .sample_count = 0,
    .read_errors = 0,
    .last_sample_ms = 0,
    .range_min_c = -10,
    .range_max_c = 80,
};

static double s_temp_sum_c = 0.0;

static uint32_t get_uptime_ms(void) {
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

#if SOC_TEMP_SENSOR_SUPPORTED
static esp_err_t install_sensor_range(int min_c, int max_c) {
    if (s_tsens) {
        if (s_status.enabled) {
            (void)temperature_sensor_disable(s_tsens);
            s_status.enabled = false;
        }
        (void)temperature_sensor_uninstall(s_tsens);
        s_tsens = NULL;
    }

    temperature_sensor_config_t cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(min_c, max_c);
    esp_err_t err = temperature_sensor_install(&cfg, &s_tsens);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "temperature_sensor_install(%d..%d) failed: %s", min_c, max_c, esp_err_to_name(err));
        s_tsens = NULL;
        s_status.initialized = false;
        return err;
    }

    err = temperature_sensor_enable(s_tsens);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "temperature_sensor_enable failed: %s", esp_err_to_name(err));
        (void)temperature_sensor_uninstall(s_tsens);
        s_tsens = NULL;
        s_status.initialized = false;
        return err;
    }

    s_status.initialized = true;
    s_status.enabled = true;
    s_status.range_min_c = min_c;
    s_status.range_max_c = max_c;
    return ESP_OK;
}
#endif

static void record_sample_locked(float temp_c, bool valid) {
    uint32_t now_ms = get_uptime_ms();
    if (!valid) {
        s_status.read_errors++;
        s_status.valid = false;
        return;
    }

    s_status.valid = true;
    s_status.current_c = temp_c;
    s_status.last_sample_ms = now_ms;
    s_status.sample_count++;
    s_temp_sum_c += (double)temp_c;
    if (temp_c < s_status.min_c) s_status.min_c = temp_c;
    if (temp_c > s_status.max_c) s_status.max_c = temp_c;
    s_status.avg_c = (float)(s_temp_sum_c / (double)s_status.sample_count);

    s_history[s_hist_head].temp_c = temp_c;
    s_history[s_hist_head].timestamp_ms = now_ms;
    s_history[s_hist_head].valid = true;
    s_hist_head = (s_hist_head + 1) % TEMP_MONITOR_HISTORY_MAX;
    if (s_hist_count < TEMP_MONITOR_HISTORY_MAX) {
        s_hist_count++;
    }
}

esp_err_t temp_monitor_sample_now(float *out_celsius) {
    if (!s_mutex) {
        s_mutex = xSemaphoreCreateMutex();
        if (!s_mutex) return ESP_ERR_NO_MEM;
    }

    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

#if SOC_TEMP_SENSOR_SUPPORTED
    if (!s_tsens) {
        /* ESP32-S3 hardware table supports [-10, 80] with <1 C error */
        if (install_sensor_range(-10, 80) != ESP_OK) {
            record_sample_locked(0.0f, false);
            xSemaphoreGive(s_mutex);
            return ESP_FAIL;
        }
    }

    float t = 0.0f;
    esp_err_t err = temperature_sensor_get_celsius(s_tsens, &t);

    /* Automatic range escalation if silicon exceeds current range ceiling */
    if (err == ESP_OK && t >= 75.0f && s_status.range_max_c <= 80) {
        if (install_sensor_range(20, 100) == ESP_OK) {
            err = temperature_sensor_get_celsius(s_tsens, &t);
        }
    } else if (err == ESP_OK && t >= 95.0f && s_status.range_max_c <= 100) {
        if (install_sensor_range(50, 125) == ESP_OK) {
            err = temperature_sensor_get_celsius(s_tsens, &t);
        }
    } else if (err == ESP_FAIL && s_status.range_min_c > -10) {
        if (install_sensor_range(-10, 80) == ESP_OK) {
            err = temperature_sensor_get_celsius(s_tsens, &t);
        }
    }

    if (err == ESP_OK && !isnan(t)) {
        record_sample_locked(t, true);
        if (out_celsius) *out_celsius = t;
        xSemaphoreGive(s_mutex);
        return ESP_OK;
    } else {
        record_sample_locked(0.0f, false);
        xSemaphoreGive(s_mutex);
        return (err != ESP_OK) ? err : ESP_FAIL;
    }
#else
    record_sample_locked(0.0f, false);
    xSemaphoreGive(s_mutex);
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static void temp_sampler_timer_cb(void *arg) {
    (void)arg;
    /* esp_timer task has limited stack; sample non-blocking via quick mutex try */
    (void)temp_monitor_sample_now(NULL);
}

esp_err_t temp_monitor_init(void) {
    if (!s_mutex) {
        s_mutex = xSemaphoreCreateMutex();
        if (!s_mutex) return ESP_ERR_NO_MEM;
    }

    float initial_c = 0.0f;
    esp_err_t err = temp_monitor_sample_now(&initial_c);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "ESP32-S3 Internal Temperature Monitor ready: %.1f C (range %d..%d C)",
                 initial_c, s_status.range_min_c, s_status.range_max_c);
    } else {
        ESP_LOGW(TAG, "Initial temperature sample failed: %s", esp_err_to_name(err));
    }

    if (!s_sample_timer) {
        const esp_timer_create_args_t targs = {
            .callback = temp_sampler_timer_cb,
            .arg = NULL,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "temp_mon",
            .skip_unhandled_events = true,
        };
        if (esp_timer_create(&targs, &s_sample_timer) == ESP_OK) {
            (void)esp_timer_start_periodic(s_sample_timer, (uint64_t)s_status.interval_ms * 1000ULL);
        }
    }
    return err;
}

void temp_monitor_get_status(temp_monitor_status_t *out_status) {
    if (!out_status) return;
    if (!s_mutex) {
        memcpy(out_status, &s_status, sizeof(*out_status));
        return;
    }
    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        memcpy(out_status, &s_status, sizeof(*out_status));
        xSemaphoreGive(s_mutex);
    } else {
        memcpy(out_status, &s_status, sizeof(*out_status));
    }
}

size_t temp_monitor_get_history(temp_sample_t *out_samples, size_t max_samples) {
    if (!out_samples || max_samples == 0 || !s_mutex) return 0;
    size_t copied = 0;
    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        size_t to_copy = (s_hist_count < max_samples) ? s_hist_count : max_samples;
        size_t start_offset = s_hist_count - to_copy;
        size_t oldest_idx = (s_hist_head + TEMP_MONITOR_HISTORY_MAX - s_hist_count) % TEMP_MONITOR_HISTORY_MAX;
        for (size_t i = 0; i < to_copy; i++) {
            size_t idx = (oldest_idx + start_offset + i) % TEMP_MONITOR_HISTORY_MAX;
            out_samples[i] = s_history[idx];
        }
        copied = to_copy;
        xSemaphoreGive(s_mutex);
    }
    return copied;
}

void temp_monitor_set_unit(temp_unit_t unit) {
    if (s_mutex && xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_status.unit = unit;
        xSemaphoreGive(s_mutex);
    } else {
        s_status.unit = unit;
    }
}

esp_err_t temp_monitor_set_interval_ms(uint32_t interval_ms) {
    if (interval_ms < 500 || interval_ms > 60000) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_mutex && xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_status.interval_ms = interval_ms;
        xSemaphoreGive(s_mutex);
    }
    if (s_sample_timer) {
        (void)esp_timer_stop(s_sample_timer);
        return esp_timer_start_periodic(s_sample_timer, (uint64_t)interval_ms * 1000ULL);
    }
    return ESP_OK;
}

void temp_monitor_reset_stats(void) {
    if (s_mutex && xSemaphoreTake(s_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        memset(s_history, 0, sizeof(s_history));
        s_hist_head = 0;
        s_hist_count = 0;
        s_status.sample_count = 0;
        s_status.read_errors = 0;
        s_status.min_c = 999.0f;
        s_status.max_c = -999.0f;
        s_status.avg_c = 0.0f;
        s_temp_sum_c = 0.0;
        xSemaphoreGive(s_mutex);
    }
}

static float to_unit(float c, temp_unit_t u) {
    return (u == TEMP_UNIT_FAHRENHEIT) ? ((c * 9.0f / 5.0f) + 32.0f) : c;
}

void temp_monitor_print_report(bool json_only) {
    float dummy = 0.0f;
    (void)temp_monitor_sample_now(&dummy);

    temp_monitor_status_t st;
    temp_monitor_get_status(&st);

    size_t free_int = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    if (json_only) {
        glog("[TEMP_JSON] {\"sensor\":\"ESP32 CHIP INTERNAL TEMPERATURE\",\"init\":%s,\"valid\":%s,"
             "\"temp_c\":%.1f,\"temp_f\":%.1f,\"min_c\":%.1f,\"max_c\":%.1f,\"avg_c\":%.1f,"
             "\"unit\":\"%s\",\"interval_ms\":%lu,\"samples\":%lu,\"errors\":%lu,\"last_ms\":%lu,"
             "\"range_min\":%d,\"range_max\":%d,\"sram_free_kb\":%u,\"psram_free_kb\":%u}\n",
             st.initialized ? "true" : "false",
             st.valid ? "true" : "false",
             st.valid ? st.current_c : 0.0f,
             st.valid ? ((st.current_c * 9.0f / 5.0f) + 32.0f) : 0.0f,
             (st.sample_count > 0) ? st.min_c : 0.0f,
             (st.sample_count > 0) ? st.max_c : 0.0f,
             (st.sample_count > 0) ? st.avg_c : 0.0f,
             st.unit == TEMP_UNIT_FAHRENHEIT ? "F" : "C",
             (unsigned long)st.interval_ms,
             (unsigned long)st.sample_count,
             (unsigned long)st.read_errors,
             (unsigned long)st.last_sample_ms,
             st.range_min_c,
             st.range_max_c,
             (unsigned)(free_int / 1024),
             (unsigned)(free_psram / 1024));
        return;
    }

    const char *u_sym = (st.unit == TEMP_UNIT_FAHRENHEIT) ? "F" : "C";
    glog("\n            ESP32 CHIP INTERNAL TEMPERATURE & TELEMETRY           \n"
         "=================================================================\n");
    if (st.valid) {
        float cur_f = (st.current_c * 9.0f / 5.0f) + 32.0f;
        const char *band = (st.current_c < 55.0f) ? "NOMINAL" :
                           (st.current_c < 75.0f) ? "WARM (RADIO LOAD)" : "ELEVATED";
        glog(" Sensor Label     : ESP32 CHIP INTERNAL TEMPERATURE\n");
        glog(" Current Die Temp : %.1f C (%.1f F) [%s]\n", st.current_c, cur_f, band);
        glog(" Min / Max / Avg  : %.1f %s / %.1f %s / %.1f %s\n",
             to_unit(st.min_c, st.unit), u_sym,
             to_unit(st.max_c, st.unit), u_sym,
             to_unit(st.avg_c, st.unit), u_sym);
        glog(" Active HW Range  : %d C to %d C (ESP-IDF calibrated curve)\n", st.range_min_c, st.range_max_c);
    } else {
        glog(" Sensor Status    : ERROR (initialized=%s, read_errors=%lu)\n",
             st.initialized ? "yes" : "no", (unsigned long)st.read_errors);
    }
    glog(" Sampler State    : interval=%lu ms | samples=%lu | errors=%lu | last=%lu ms\n",
         (unsigned long)st.interval_ms, (unsigned long)st.sample_count,
         (unsigned long)st.read_errors, (unsigned long)st.last_sample_ms);
    glog(" Internal SRAM    : %u KB free\n", (unsigned)(free_int / 1024));
    glog(" Octal PSRAM      : %u KB free\n", (unsigned)(free_psram / 1024));
    glog(" Calibration Note : Measures internal silicon junction, NOT ambient air.\n"
         "                    Continuous Wi-Fi SoftAP/RX normally runs 45-60 C.\n"
         "=================================================================\n\n");
}

void temp_monitor_print_history(void) {
    temp_sample_t buf[30];
    size_t n = temp_monitor_get_history(buf, 30);
    glog("\n       ESP32 CHIP INTERNAL TEMPERATURE HISTORY (%u samples)       \n"
         "-----------------------------------------------------------------\n", (unsigned)n);
    for (size_t i = 0; i < n; i++) {
        int bars = (int)((buf[i].temp_c - 20.0f) / 2.0f);
        if (bars < 1) bars = 1;
        if (bars > 30) bars = 30;
        char bar_str[32];
        memset(bar_str, '#', bars);
        bar_str[bars] = '\0';
        glog(" %7lu ms | %5.1f C | %-30s\n",
             (unsigned long)buf[i].timestamp_ms, buf[i].temp_c, bar_str);
    }
    glog("-----------------------------------------------------------------\n\n");
}
