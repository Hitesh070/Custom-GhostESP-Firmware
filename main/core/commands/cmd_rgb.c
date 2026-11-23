// cmd_rgb.c
// RGB LED and Neopixel brightness commands.

#include "core/commands.h"
#include "core/glog.h"
#include "managers/rgb_manager.h"
#include "managers/settings_manager.h"
#include "managers/status_display_manager.h"
#include "managers/display_manager.h"
#include "managers/views/error_popup.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handle_set_rgb_mode_cmd(int argc, char **argv) {
    if (argc != 2) {
        glog("Usage: setrgbmode <normal|rainbow|stealth>\n");
        return;
    }
    RGBMode mode;
    if (strcasecmp(argv[1], "normal") == 0) {
        mode = RGB_MODE_NORMAL;
    } else if (strcasecmp(argv[1], "rainbow") == 0) {
        mode = RGB_MODE_RAINBOW;
    } else if (strcasecmp(argv[1], "stealth") == 0) {
        mode = RGB_MODE_STEALTH;
    } else {
        glog("Invalid mode '%s'. Supported modes: normal, rainbow, stealth\n", argv[1]);
        return;
    }
    settings_set_rgb_mode(&G_Settings, mode);
    settings_save(&G_Settings);
    glog("RGB mode set to %s\n", argv[1]);
}

void handle_set_neopixel_brightness_cmd(int argc, char **argv) {
    if (argc != 2) {
        glog("Usage: setneopixelbrightness <0-100>\n");
        glog("Example: setneopixelbrightness 50\n");
        return;
    }
    
    int brightness = atoi(argv[1]);
    if (brightness < 0 || brightness > 100) {
        glog("Invalid brightness value '%s'. Must be between 0-100\n", argv[1]);
        return;
    }
    
    settings_set_neopixel_max_brightness(&G_Settings, (uint8_t)brightness);
    settings_save(&G_Settings);
    RGBMode mode = settings_get_rgb_mode(&G_Settings);
    if (mode >= RGB_MODE_RED && mode <= RGB_MODE_PINK) {
        rgb_manager_apply_static_from_settings();
    }
    glog("Neopixel max brightness set to %d%%\n", brightness);
}

void handle_get_neopixel_brightness_cmd(int argc, char **argv) {
    uint8_t brightness = settings_get_neopixel_max_brightness(&G_Settings);
    glog("Current neopixel max brightness: %d%%\n", brightness);
}

static void show_rgb_warning_popup(void *arg) {
    (void)arg;
    if (display_manager_is_available()) {
        error_popup_create("EPILEPSY WARNING\nRapid flashing lights");
    }
}

void handle_rgb_mode(int argc, char **argv) {
    if (argc < 2) {
        glog("Usage: rgbmode <rainbow|police|strobe|knight|off|color>\n");
        status_display_show_status("RGB Usage");
        return;
    }

    // Cancel any currently running LED effect task safely.
    if (rgb_effect_task_handle != NULL) {
        rgb_manager_signal_rainbow_exit();
        int wait_ms = 0;
        while (rgb_effect_task_handle != NULL && wait_ms < 1000) {
            vTaskDelay(pdMS_TO_TICKS(10));
            wait_ms += 10;
        }
        if (rgb_effect_task_handle != NULL) {
            ESP_LOGW("cmd_rgb", "Previous effect task timed out waiting to exit, clearing handle");
            rgb_effect_task_handle = NULL;
        }
    }

    // Check for built-in modes first.
    if (strcasecmp(argv[1], "rainbow") == 0) {
        if (!(rgb_manager.is_separate_pins || rgb_manager.strip)) {
            glog("RGB not initialized\n");
            status_display_show_status("RGB Not Ready");
            return;
        }
        BaseType_t res = xTaskCreate(rainbow_task, "rainbow_effect", 4096, &rgb_manager, 5, &rgb_effect_task_handle);
        if (res == pdPASS) {
            glog("Rainbow mode activated\n");
            status_display_show_status("RGB Rainbow");
        } else {
            glog("Failed to create Rainbow task\n");
        }
    } else if (strcasecmp(argv[1], "police") == 0) {
        if (settings_get_epilepsy_warning_enabled(&G_Settings)) {
            glog("WARNING: Epilepsy risk - rapid flashing lights\n");
            if (display_manager_is_available()) {
                display_manager_run_on_lvgl(show_rgb_warning_popup, NULL);
                vTaskDelay(pdMS_TO_TICKS(2000));
            }
        }
        if (!(rgb_manager.is_separate_pins || rgb_manager.strip)) {
            glog("RGB not initialized\n");
            status_display_show_status("RGB Not Ready");
            return;
        }
        BaseType_t res = xTaskCreate(police_task, "police_effect", 4096, &rgb_manager, 5, &rgb_effect_task_handle);
        if (res == pdPASS) {
            glog("Police mode activated\n");
            status_display_show_status("RGB Police");
        } else {
            glog("Failed to create Police task\n");
        }
    } else if (strcasecmp(argv[1], "strobe") == 0) {
        if (settings_get_epilepsy_warning_enabled(&G_Settings)) {
            glog("WARNING: Epilepsy risk - rapid flashing lights\n");
            if (display_manager_is_available()) {
                display_manager_run_on_lvgl(show_rgb_warning_popup, NULL);
                vTaskDelay(pdMS_TO_TICKS(2000));
            }
        }
        if (!(rgb_manager.is_separate_pins || rgb_manager.strip)) {
            glog("RGB not initialized\n");
            status_display_show_status("RGB Not Ready");
            return;
        }
        BaseType_t res = xTaskCreate(strobe_task, "strobe_effect", 4096, &rgb_manager, 5, &rgb_effect_task_handle);
        if (res == pdPASS) {
            glog("Strobe mode activated\n");
            status_display_show_status("RGB Strobe");
        } else {
            glog("Failed to create Strobe task\n");
        }
    } else if (strcasecmp(argv[1], "knight") == 0) {
        if (!(rgb_manager.is_separate_pins || rgb_manager.strip)) {
            glog("RGB not initialized\n");
            status_display_show_status("RGB Not Ready");
            return;
        }
        BaseType_t res = xTaskCreate(knightrider_task, "knightrider_effect", 4096, &rgb_manager, 5, &rgb_effect_task_handle);
        if (res == pdPASS) {
            glog("Knight Rider mode activated\n");
            status_display_show_status("RGB Knight");
        } else {
            glog("Failed to create Knight Rider task\n");
        }
    } else if (strcasecmp(argv[1], "off") == 0) {
        rgb_manager_set_color(&rgb_manager, -1, 0, 0, 0, false);
        if (!rgb_manager.is_separate_pins && rgb_manager.strip) {
            led_strip_clear(rgb_manager.strip);
            led_strip_refresh(rgb_manager.strip);
        }
        settings_set_rgb_mode(&G_Settings, RGB_MODE_NORMAL);
        settings_save(&G_Settings);
        glog("RGB disabled\n");
        status_display_show_status("RGB Off");
    } else {
        // Otherwise, treat the argument as a color name.
        typedef struct {
            const char *name;
            uint8_t r;
            uint8_t g;
            uint8_t b;
            RGBMode mode;
        } color_t;
        static const color_t supported_colors[] = {
            { "red",         255, 0,   0,   RGB_MODE_RED },
            { "green",       0,   255, 0,   RGB_MODE_GREEN },
            { "blue",        0,   0,   255, RGB_MODE_BLUE },
            { "yellow",      255, 255, 0,   RGB_MODE_YELLOW },
            { "twh-purple",  115, 0,   225, RGB_MODE_PURPLE }, // #7300E1
            { "cyan",        0,   255, 255, RGB_MODE_CYAN },
            { "orange",      255, 165, 0,   RGB_MODE_ORANGE },
            { "white",       255, 255, 255, RGB_MODE_WHITE },
            { "pink",        255, 192, 203, RGB_MODE_PINK }
        };
        const int num_colors = sizeof(supported_colors) / sizeof(supported_colors[0]);
        int found = 0;
        uint8_t r, g, b;
        RGBMode chosen_mode = RGB_MODE_NORMAL;
        for (int i = 0; i < num_colors; i++) {
            // Use case-insensitive compare.
            if (strcasecmp(argv[1], supported_colors[i].name) == 0) {
                r = supported_colors[i].r;
                g = supported_colors[i].g;
                b = supported_colors[i].b;
                chosen_mode = supported_colors[i].mode;
                found = 1;
                break;
            }
        }
        if (!found) {
            glog("Unknown color '%s'. Supported colors: red, green, blue, yellow, twh-purple, cyan, orange, white, pink.\n", argv[1]);
            status_display_show_status("Color Invalid");
            return;
        }
        // Set each LED to the selected static color.
        for (int i = 0; i < rgb_manager.num_leds; i++) {
            rgb_manager_set_color(&rgb_manager, i, r, g, b, false);
        }
        led_strip_refresh(rgb_manager.strip);
        // Persist selection so it remains active after other effects/off are toggled
        settings_set_rgb_mode(&G_Settings, chosen_mode);
        settings_save(&G_Settings);
        glog("Static color mode activated: %s\n", argv[1]);
        status_display_show_status("RGB Static");
    }
}

void handle_setrgb(int argc, char **argv) {
    if (argc != 4) {
        glog("Usage: setrgbpins <red> <green> <blue>\n");
        glog("           (use same value for all pins for single-pin LED strips)\n\n");
        status_display_show_status("SetRGB Usage");
        return;
    }
    gpio_num_t red_pin = (gpio_num_t)atoi(argv[1]);
    gpio_num_t green_pin = (gpio_num_t)atoi(argv[2]);
    gpio_num_t blue_pin = (gpio_num_t)atoi(argv[3]);

    int num_leds = settings_get_rgb_led_count(&G_Settings);
    if (num_leds <= 0) {
        if (rgb_manager.num_leds > 0) {
            num_leds = rgb_manager.num_leds;
        } else if (CONFIG_NUM_LEDS > 0) {
            num_leds = CONFIG_NUM_LEDS;
        } else {
            num_leds = 1;
        }
    }

    esp_err_t ret;
    if (red_pin == green_pin && green_pin == blue_pin) {
        rgb_manager_deinit(&rgb_manager);
        ret = rgb_manager_init(&rgb_manager, red_pin, num_leds, LED_PIXEL_FORMAT_GRB, LED_MODEL_WS2812,
                               GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC);
        if (ret == ESP_OK) {
            settings_set_rgb_data_pin(&G_Settings, red_pin);
            settings_set_rgb_separate_pins(&G_Settings, -1, -1, -1);
            settings_save(&G_Settings);
            glog("Single-pin RGB configured on GPIO %d and saved.\n", red_pin);
            status_display_show_status("RGB Single");
        } else {
            glog("Failed to init RGB on pin %d: %s\n", red_pin, esp_err_to_name(ret));
            status_display_show_status("RGB Init Fail");
        }
    } else {
        rgb_manager_deinit(&rgb_manager);
        ret = rgb_manager_init(&rgb_manager, GPIO_NUM_NC, num_leds, LED_PIXEL_FORMAT_GRB, LED_MODEL_WS2812,
                               red_pin, green_pin, blue_pin);
        if (ret == ESP_OK) {
            settings_set_rgb_data_pin(&G_Settings, -1);
            settings_set_rgb_separate_pins(&G_Settings, red_pin, green_pin, blue_pin);
            settings_save(&G_Settings);
            glog("RGB pins updated to R:%d G:%d B:%d and saved.\n", red_pin, green_pin, blue_pin);
            status_display_show_status("RGB Pins Set");
        } else {
            glog("Failed to init RGB pins R:%d G:%d B:%d: %s\n", red_pin, green_pin, blue_pin, esp_err_to_name(ret));
            status_display_show_status("RGB Init Fail");
        }
    }
}

void handle_setrgbcount(int argc, char **argv) {
    if (argc != 2) {
        glog("Usage: setrgbcount <1-512>\n");
        status_display_show_status("Count Usage");
        return;
    }

    int count = atoi(argv[1]);
    if (count < 1 || count > 512) {
        glog("RGB LED count must be between 1 and 512\n");
        status_display_show_status("Count Invalid");
        return;
    }

    settings_set_rgb_led_count(&G_Settings, (uint16_t)count);

    int32_t data_pin = settings_get_rgb_data_pin(&G_Settings);
    int32_t red_pin, green_pin, blue_pin;
    settings_get_rgb_separate_pins(&G_Settings, &red_pin, &green_pin, &blue_pin);

    esp_err_t ret = ESP_OK;
    bool attempted_reinit = false;

    if (data_pin != GPIO_NUM_NC) {
        rgb_manager_deinit(&rgb_manager);
        ret = rgb_manager_init(&rgb_manager, (gpio_num_t)data_pin, count, LED_PIXEL_FORMAT_GRB,
                               LED_MODEL_WS2812, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC);
        attempted_reinit = true;
    } else if (red_pin != GPIO_NUM_NC && green_pin != GPIO_NUM_NC && blue_pin != GPIO_NUM_NC) {
        rgb_manager_deinit(&rgb_manager);
        ret = rgb_manager_init(&rgb_manager, GPIO_NUM_NC, count, LED_PIXEL_FORMAT_GRB,
                               LED_MODEL_WS2812, (gpio_num_t)red_pin, (gpio_num_t)green_pin, (gpio_num_t)blue_pin);
        attempted_reinit = true;
    }

    settings_save(&G_Settings);

    if (attempted_reinit && ret == ESP_OK) {
        glog("RGB LED count set to %d and applied.\n", count);
        status_display_show_status("RGB Count Set");
    } else if (attempted_reinit) {
        glog("RGB count saved but reinit failed: %s\n", esp_err_to_name(ret));
        status_display_show_status("RGB Reinit NG");
    } else {
        glog("RGB count set to %d. Configure pins with setrgbpins to apply.\n", count);
        status_display_show_status("RGB Count Saved");
    }
}
