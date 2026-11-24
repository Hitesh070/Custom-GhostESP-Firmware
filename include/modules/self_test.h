#ifndef MODULES_SELF_TEST_H
#define MODULES_SELF_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    TEST_RESULT_PASS = 0,
    TEST_RESULT_FAIL,
    TEST_RESULT_WARNING,
    TEST_RESULT_HW_DEPENDENT,
    TEST_RESULT_NOT_TESTED
} test_result_t;

typedef struct {
    const char   *subsystem;
    const char   *test_name;
    test_result_t result;
    char          details[64];
} test_item_t;

/**
 * @brief Run the complete God Mode V3 automated hardware & firmware self-test.
 *
 * Evaluates internal heap, external Octal PSRAM, I2C0, SPI2 (NRF24),
 * SPI3 (Display), SD card, and RGB indicators.
 */
void self_test_run_all(void);

#ifdef __cplusplus
}
#endif

#endif /* MODULES_SELF_TEST_H */
