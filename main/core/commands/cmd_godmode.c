#include "modules/rogue_detector.h"
#include "modules/deauth_monitor.h"
#include "modules/nrf24_lab.h"
#include "modules/profile_manager.h"
#include "modules/self_test.h"
#include "modules/ghost_eastereggs.h"
#include "core/serial_manager.h"
#include "core/glog.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void handle_rogue_cmd(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "status") == 0) {
        rogue_detector_dump_status();
        return;
    }
    if (strcmp(argv[1], "start") == 0) {
        rogue_detector_start();
        glog("[+] Passive Rogue AP & Evil Twin Detection STARTED\n");
    } else if (strcmp(argv[1], "stop") == 0) {
        rogue_detector_stop();
        glog("[+] Passive Rogue AP & Evil Twin Detection STOPPED\n");
    } else if (strcmp(argv[1], "clear") == 0) {
        rogue_detector_clear();
        glog("[+] Rogue AP candidate database CLEARED\n");
    } else {
        glog("Usage: roguedetect [start|stop|status|clear]\n");
    }
}

void handle_deauthmon_cmd(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "status") == 0) {
        deauth_monitor_dump_status();
        return;
    }
    if (strcmp(argv[1], "start") == 0) {
        deauth_monitor_start();
        glog("[+] Passive Deauthentication Activity Monitor STARTED\n");
    } else if (strcmp(argv[1], "stop") == 0) {
        deauth_monitor_stop();
        glog("[+] Passive Deauthentication Activity Monitor STOPPED\n");
    } else if (strcmp(argv[1], "clear") == 0) {
        deauth_monitor_clear();
        glog("[+] Deauth statistics and history CLEARED\n");
    } else {
        glog("Usage: deauthmon [start|stop|status|clear]\n");
    }
}

void handle_nrf24lab_cmd(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "diag") == 0 || strcmp(argv[1], "regs") == 0) {
        nrf24_lab_dump_registers();
        return;
    }
    if (strcmp(argv[1], "sweep") == 0) {
        uint32_t samples = (argc >= 3) ? (uint32_t)atoi(argv[2]) : 10;
        nrf24_lab_run_channel_sweep(samples);
    } else if (strcmp(argv[1], "benchmark") == 0) {
        uint32_t pkts = (argc >= 3) ? (uint32_t)atoi(argv[2]) : 100;
        nrf24_lab_run_benchmark(pkts);
    } else {
        glog("Usage: nrf24lab [diag|regs|sweep <samples>|benchmark <pkts>]\n");
    }
}

void handle_profile_cmd(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "list") == 0) {
        profile_manager_dump();
        return;
    }
    if (strcmp(argv[1], "apply") == 0 || strcmp(argv[1], "set") == 0) {
        if (argc < 3) {
            glog("Error: Profile ID required. Usage: profile apply <0-7>\n");
            return;
        }
        int id = atoi(argv[2]);
        if (id < 0 || id >= PROFILE_ID_MAX) {
            glog("Invalid profile ID %d (Valid range: 0 to %d)\n", id, PROFILE_ID_MAX - 1);
            return;
        }
        profile_manager_apply((profile_id_t)id);
    } else {
        glog("Usage: profile [list|apply <id>]\n");
    }
}

void handle_selftest_cmd(int argc, char **argv) {
    (void)argc;
    (void)argv;
    self_test_run_all();
    ghost_unlock_achievement(ACH_FIRST_SELFTEST);
}

#include "esp_heap_caps.h"
#if SOC_TEMP_SENSOR_SUPPORTED
#include "driver/temperature_sensor.h"
#endif

void handle_ghost_cmd(int argc, char **argv) {
    ghost_handle_developer_command(argc, argv);
}

void handle_temp_cmd(int argc, char **argv) {
    (void)argc;
    (void)argv;
    float temp_c = -999.0f;
#if SOC_TEMP_SENSOR_SUPPORTED
    static temperature_sensor_handle_t temp_sensor = NULL;
    if (!temp_sensor) {
        temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 85);
        if (temperature_sensor_install(&temp_sensor_config, &temp_sensor) == ESP_OK) {
            temperature_sensor_enable(temp_sensor);
        }
    }
    if (temp_sensor) {
        temperature_sensor_get_celsius(temp_sensor, &temp_c);
    }
#endif
    size_t free_int = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    glog("\n               GHOSTESP HARDWARE & THERMAL TELEMETRY              \n"
         "=================================================================\n");
    if (temp_c > -100.0f) {
        float temp_f = (temp_c * 9.0f / 5.0f) + 32.0f;
        glog(" Core Die Temp    : %.1f C (%.1f F) [%s]\n",
             temp_c, temp_f,
             (temp_c < 60.0f) ? "NOMINAL / COOL" : (temp_c < 75.0f ? "WARM (NORMAL UNDER RADIO LOAD)" : "HOT"));
    } else {
        glog(" Core Die Temp    : N/A\n");
    }
    glog(" Operating Clock  : 240 MHz (Dual-Core Xtensa LX7)\n");
    glog(" Internal SRAM    : %u KB free\n", (unsigned)(free_int / 1024));
    glog(" Octal PSRAM      : %u MB free\n", (unsigned)(free_psram / (1024 * 1024)));
    glog(" Thermal Note     : ESP32-S3 silicon is rated up to 105 C.\n"
         "                    Under continuous 240MHz and Wi-Fi SoftAP/RX,\n"
         "                    operating temperatures between 45 C and 60 C are normal.\n"
         "                    Use 'apenable 0' to disable SoftAP and cool down.\n"
         "=================================================================\n\n");
}
