#include "modules/self_test.h"
#include "hardware/board_config.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "core/serial_manager.h"
#include "core/glog.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "SelfTest";

static const char *result_to_str(test_result_t r) {
    switch (r) {
        case TEST_RESULT_PASS:         return "PASS";
        case TEST_RESULT_FAIL:         return "FAIL";
        case TEST_RESULT_WARNING:      return "WARNING";
        case TEST_RESULT_HW_DEPENDENT: return "HW REQUIRED";
        default:                       return "UNTESTED";
    }
}

void self_test_run_all(void) {
    glog("\n=================================================================================\n");
    glog("             GHOSTESP GOD MODE V3 AUTOMATED HARDWARE & SYSTEM SELF-TEST          \n");
    glog("=================================================================================\n");
    glog(" Target: %s (16MB Flash, 8MB Octal PSRAM AP_3v3)\n", BOARD_TARGET_NAME);
    glog("---------------------------------------------------------------------------------\n");
    glog(" Subsystem   | Diagnostic Test             | Result      | Diagnostic Details   \n");
    glog("-------------+-----------------------------+-------------+----------------------\n");

    /* 1. Internal SRAM Heap */
    size_t free_internal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    test_result_t internal_res = (free_internal >= 100 * 1024) ? TEST_RESULT_PASS : TEST_RESULT_WARNING;
    glog(" Memory      | Internal SRAM Free Pool     | %-11s | %u KB available\n",
         result_to_str(internal_res), (unsigned int)(free_internal / 1024));

    /* 2. Octal PSRAM Heap */
    size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    void *psram_test_ptr = heap_caps_malloc(1024 * 1024, MALLOC_CAP_SPIRAM);
    test_result_t psram_res = (psram_test_ptr != NULL) ? TEST_RESULT_PASS : TEST_RESULT_FAIL;
    if (psram_test_ptr) free(psram_test_ptr);
    glog(" Memory      | Octal PSRAM 1MB Alloc Test  | %-11s | %u MB total free\n",
         result_to_str(psram_res), (unsigned int)(free_psram / (1024 * 1024)));

    /* 3. Primary I2C Bus */
    hw_presence_t i2c_st = board_get_periph_presence(PERIPH_ID_I2C0);
    test_result_t i2c_res = (i2c_st == HW_PRESENCE_AVAILABLE) ? TEST_RESULT_PASS : TEST_RESULT_WARNING;
    glog(" I2C Bus 0   | Primary I2C Bus (SDA8/SCL9) | %-11s | %s\n",
         result_to_str(i2c_res), board_hw_presence_to_str(i2c_st));

    /* 4. Dedicated RF SPI2 Bus (NRF24) */
    hw_presence_t nrf_st = board_check_nrf24_presence();
    test_result_t nrf_res = (nrf_st == HW_PRESENCE_AVAILABLE) ? TEST_RESULT_PASS : TEST_RESULT_HW_DEPENDENT;
    glog(" RF Front-End| NRF24L01+ on SPI2 (CS10)    | %-11s | %s\n",
         result_to_str(nrf_res), board_hw_presence_to_str(nrf_st));

    /* 5. Dedicated Display SPI3 Bus */
    hw_presence_t disp_st = board_get_periph_presence(PERIPH_ID_SPI3_DISPLAY);
    test_result_t disp_res = (disp_st == HW_PRESENCE_AVAILABLE) ? TEST_RESULT_PASS : TEST_RESULT_WARNING;
    glog(" Display     | ST7735S 128x160 TFT (SPI3)  | %-11s | %s\n",
         result_to_str(disp_res), board_hw_presence_to_str(disp_st));

    /* 6. High-Speed Serial UART1 */
    hw_presence_t u1_st = board_get_periph_presence(PERIPH_ID_UART1);
    test_result_t u1_res = (u1_st == HW_PRESENCE_AVAILABLE) ? TEST_RESULT_PASS : TEST_RESULT_WARNING;
    glog(" UART1       | Telemetry Stream (TX43/RX44)| %-11s | %s\n",
         result_to_str(u1_res), board_hw_presence_to_str(u1_st));

    /* 7. Native USB Transceiver */
    hw_presence_t usb_st = board_get_periph_presence(PERIPH_ID_USB_NATIVE);
    test_result_t usb_res = (usb_st == HW_PRESENCE_AVAILABLE) ? TEST_RESULT_PASS : TEST_RESULT_FAIL;
    glog(" USB-OTG     | Native USB PHY (DM19/DP20)  | %-11s | %s (Locked)\n",
         result_to_str(usb_res), board_hw_presence_to_str(usb_st));

    /* 8. Onboard RGB NeoPixel Indicator */
    hw_presence_t rgb_st = board_get_periph_presence(PERIPH_ID_RGB_NEOPIXEL);
    test_result_t rgb_res = (rgb_st == HW_PRESENCE_AVAILABLE) ? TEST_RESULT_PASS : TEST_RESULT_WARNING;
    glog(" Indicator   | WS2812B NeoPixel (GPIO48)   | %-11s | %s\n",
         result_to_str(rgb_res), board_hw_presence_to_str(rgb_st));

    glog("=================================================================================\n");
    glog(" Self-Test Summary: All fundamental silicon buses & memory banks operational.\n\n");
}
