#ifndef MODULES_TEMP_MONITOR_H
#define MODULES_TEMP_MONITOR_H

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEMP_MONITOR_HISTORY_MAX 120
#define TEMP_MONITOR_DEFAULT_INTERVAL_MS 2000

typedef enum {
    TEMP_UNIT_CELSIUS = 0,
    TEMP_UNIT_FAHRENHEIT = 1,
} temp_unit_t;

typedef struct {
    float temp_c;
    uint32_t timestamp_ms;
    bool valid;
} temp_sample_t;

typedef struct {
    bool initialized;
    bool enabled;
    bool valid;
    float current_c;
    float min_c;
    float max_c;
    float avg_c;
    temp_unit_t unit;
    uint32_t interval_ms;
    uint32_t sample_count;
    uint32_t read_errors;
    uint32_t last_sample_ms;
    int range_min_c;
    int range_max_c;
} temp_monitor_status_t;

/**
 * @brief Initialize ESP32-S3 on-chip internal temperature sensor & periodic sampler.
 */
esp_err_t temp_monitor_init(void);

/**
 * @brief Perform a single non-blocking on-chip temperature read and update statistics.
 */
esp_err_t temp_monitor_sample_now(float *out_celsius);

/**
 * @brief Get current snapshot of temperature monitor status & statistics.
 */
void temp_monitor_get_status(temp_monitor_status_t *out_status);

/**
 * @brief Copy recent temperature history samples (oldest to newest).
 */
size_t temp_monitor_get_history(temp_sample_t *out_samples, size_t max_samples);

/**
 * @brief Set display unit preference (Celsius or Fahrenheit).
 */
void temp_monitor_set_unit(temp_unit_t unit);

/**
 * @brief Set periodic background sampling interval in milliseconds (500 - 60000 ms).
 */
esp_err_t temp_monitor_set_interval_ms(uint32_t interval_ms);

/**
 * @brief Reset min/max/avg statistics and sample history buffer.
 */
void temp_monitor_reset_stats(void);

/**
 * @brief Print formatted console telemetry or structured JSON line.
 */
void temp_monitor_print_report(bool json_only);

/**
 * @brief Print bounded temperature history graph/table to console.
 */
void temp_monitor_print_history(void);

#ifdef __cplusplus
}
#endif

#endif /* MODULES_TEMP_MONITOR_H */
