#include "modules/self_test.h"
#include "hardware/board_config.h"
#include "managers/sd_card_manager.h"
#include "modules/temp_monitor.h"
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
    glog("             GHOSTESP GOD MODE V4 AUTOMATED HARDWARE & SYSTEM SELF-TEST          \n");
    glog("=================================================================================\n");
    glog(" Target: %s (16MB Flash, 8MB Octal PSRAM AP_3v3)\n", BOARD_TARGET_NAME);
    glog("---------------------------------------------------------------------------------\n");
    glog(" Subsystem   | Diagnostic Test             | Result      | Diagnostic Details   \n");
    glog("-------------+-----------------------------+-------------+----------------------\n");

    int warn_cnt = 0;
    int fail_cnt = 0;
    int hw_req_cnt = 0;

    /* 1. Internal SRAM Heap */
    size_t free_internal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    test_result_t internal_res = (free_internal >= 40 * 1024) ? TEST_RESULT_PASS : TEST_RESULT_WARNING;
    if (internal_res == TEST_RESULT_WARNING) warn_cnt++;
    glog(" Memory      | Internal SRAM Free Pool     | %-11s | %u KB available\n",
         result_to_str(internal_res), (unsigned int)(free_internal / 1024));

    /* 2. Octal PSRAM Heap */
    size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    void *psram_test_ptr = heap_caps_malloc(1024 * 1024, MALLOC_CAP_SPIRAM);
    test_result_t psram_res = (psram_test_ptr != NULL) ? TEST_RESULT_PASS : TEST_RESULT_FAIL;
    if (psram_test_ptr) free(psram_test_ptr);
    else fail_cnt++;
    glog(" Memory      | Octal PSRAM 1MB Alloc Test  | %-11s | %u MB total free\n",
         result_to_str(psram_res), (unsigned int)(free_psram / (1024 * 1024)));

    /* 3. Internal Silicon Temperature Sensor */
    float die_c = 0.0f;
    esp_err_t temp_err = temp_monitor_sample_now(&die_c);
    test_result_t temp_res = (temp_err == ESP_OK) ? TEST_RESULT_PASS : TEST_RESULT_FAIL;
    if (temp_res == TEST_RESULT_FAIL) fail_cnt++;
    glog(" Thermal     | On-Chip Die Temp Sensor     | %-11s | %.1f C (%s)\n",
         result_to_str(temp_res), die_c, (temp_err == ESP_OK) ? "Verified" : esp_err_to_name(temp_err));

    /* 4. Primary I2C Bus real slave probe */
    uint8_t i2c_addrs[8] = {0};
    int i2c_found = board_scan_i2c_bus(i2c_addrs, 8);
    test_result_t i2c_res = (i2c_found > 0) ? TEST_RESULT_PASS :
                            (i2c_found == 0 ? TEST_RESULT_HW_DEPENDENT : TEST_RESULT_WARNING);
    if (i2c_res == TEST_RESULT_HW_DEPENDENT) hw_req_cnt++;
    else if (i2c_res == TEST_RESULT_WARNING) warn_cnt++;
    if (i2c_found > 0) {
        glog(" I2C Bus 0   | Primary I2C Probe (8/9)     | %-11s | %d device(s) ACK (first 0x%02X)\n",
             result_to_str(i2c_res), i2c_found, i2c_addrs[0]);
    } else if (i2c_found == 0) {
        glog(" I2C Bus 0   | Primary I2C Probe (8/9)     | %-11s | 0 external ACKs on SDA8/SCL9\n",
             result_to_str(i2c_res));
    } else {
        glog(" I2C Bus 0   | Primary I2C Probe (8/9)     | %-11s | Bus busy or uninitialized\n",
             result_to_str(i2c_res));
    }

    /* 5. Dedicated RF SPI2 Bus (NRF24) */
    hw_presence_t nrf_st = board_check_nrf24_presence();
    test_result_t nrf_res = (nrf_st == HW_PRESENCE_AVAILABLE) ? TEST_RESULT_PASS : TEST_RESULT_HW_DEPENDENT;
    if (nrf_res == TEST_RESULT_HW_DEPENDENT) hw_req_cnt++;
    glog(" RF Front-End| NRF24L01+ on SPI2 (CS10)    | %-11s | %s\n",
         result_to_str(nrf_res), board_hw_presence_to_str(nrf_st));

    /* 6. Dedicated Display SPI3 Bus (write-only unless panel MISO wired) */
    hw_presence_t disp_st = board_get_periph_presence(PERIPH_ID_SPI3_DISPLAY);
    test_result_t disp_res = (disp_st == HW_PRESENCE_AVAILABLE) ? TEST_RESULT_PASS : TEST_RESULT_HW_DEPENDENT;
    if (disp_res == TEST_RESULT_HW_DEPENDENT) hw_req_cnt++;
    glog(" Display     | ST7735S 128x160 TFT (SPI3)  | %-11s | Pins Locked (No MISO readback)\n",
         result_to_str(disp_res));

    /* 7. SD Card Mount Status */
    test_result_t sd_res = sd_card_manager.is_initialized ? TEST_RESULT_PASS : TEST_RESULT_HW_DEPENDENT;
    if (sd_res == TEST_RESULT_HW_DEPENDENT) hw_req_cnt++;
    glog(" Storage     | MicroSD Card Slot           | %-11s | %s\n",
         result_to_str(sd_res), sd_card_manager.is_initialized ? "Mounted" : "No Card / Not Mounted");

    /* 8. High-Speed Serial UART1 */
    hw_presence_t u1_st = board_get_periph_presence(PERIPH_ID_UART1);
    test_result_t u1_res = (u1_st == HW_PRESENCE_AVAILABLE) ? TEST_RESULT_PASS : TEST_RESULT_WARNING;
    glog(" UART1       | Console Stream (TX43/RX44)  | %-11s | %s\n",
         result_to_str(u1_res), board_hw_presence_to_str(u1_st));

    /* 9. Native USB Transceiver */
    hw_presence_t usb_st = board_get_periph_presence(PERIPH_ID_USB_NATIVE);
    test_result_t usb_res = (usb_st == HW_PRESENCE_AVAILABLE) ? TEST_RESULT_PASS : TEST_RESULT_FAIL;
    if (usb_res == TEST_RESULT_FAIL) fail_cnt++;
    glog(" USB-OTG     | Native USB PHY (DM19/DP20)  | %-11s | %s (Locked)\n",
         result_to_str(usb_res), board_hw_presence_to_str(usb_st));

    /* 10. Onboard RGB NeoPixel Indicator */
    hw_presence_t rgb_st = board_get_periph_presence(PERIPH_ID_RGB_NEOPIXEL);
    test_result_t rgb_res = (rgb_st == HW_PRESENCE_AVAILABLE) ? TEST_RESULT_PASS : TEST_RESULT_WARNING;
    glog(" Indicator   | WS2812B NeoPixel (GPIO48)   | %-11s | %s\n",
         result_to_str(rgb_res), board_hw_presence_to_str(rgb_st));

    glog("=================================================================================\n");
    glog(" Self-Test Summary: Fails=%d, Warnings=%d, HW Required (External/Unplugged)=%d\n\n",
         fail_cnt, warn_cnt, hw_req_cnt);
}
