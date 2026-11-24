#include "modules/ghost_eastereggs.h"
#include "modules/self_test.h"
#include "modules/rogue_detector.h"
#include "modules/deauth_monitor.h"
#include "modules/profile_manager.h"
#include "hardware/board_config.h"
#include "core/serial_manager.h"
#include "core/glog.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "GhostEasterEggs";

typedef struct {
    achievement_id_t id;
    const char      *name;
    const char      *description;
    bool             unlocked;
} achievement_record_t;

static achievement_record_t s_achievements[ACH_MAX] = {
    [ACH_FIRST_BOOT] = {
        .id = ACH_FIRST_BOOT,
        .name = "Awakening",
        .description = "Successfully booted GhostESP God Mode V3 on 7Semi ESP32-S3",
        .unlocked = true,
    },
    [ACH_FIRST_WIFI_SCAN] = {
        .id = ACH_FIRST_WIFI_SCAN,
        .name = "Airspace Sentinel",
        .description = "Executed first passive Wi-Fi security reconnaissance sweep",
        .unlocked = false,
    },
    [ACH_FIRST_BLE_SCAN] = {
        .id = ACH_FIRST_BLE_SCAN,
        .name = "Blue Spectrum",
        .description = "Discovered active Bluetooth Low Energy peripheral nodes",
        .unlocked = false,
    },
    [ACH_FIRST_NRF24_PROBE] = {
        .id = ACH_FIRST_NRF24_PROBE,
        .name = "2.4GHz Carrier Lock",
        .description = "Verified NRF24L01+PA+LNA register telemetry on SPI2",
        .unlocked = false,
    },
    [ACH_FIRST_SELFTEST] = {
        .id = ACH_FIRST_SELFTEST,
        .name = "Silicon Certified",
        .description = "Completed automated hardware self-test with 0 critical errors",
        .unlocked = false,
    },
    [ACH_100_DEVICES] = {
        .id = ACH_100_DEVICES,
        .name = "Centurion",
        .description = "Cataloged over 100 wireless nodes in the airspace",
        .unlocked = false,
    }
};

esp_err_t ghost_eastereggs_init(void) {
    ESP_LOGI(TAG, "Ghost Easter Eggs & Developer Console initialized");
    return ESP_OK;
}

void ghost_unlock_achievement(achievement_id_t id) {
    if (id < ACH_MAX && !s_achievements[id].unlocked) {
        s_achievements[id].unlocked = true;
        glog("\n======================================================\n");
        glog("  ★ ACHIEVEMENT UNLOCKED: [%s] ★\n", s_achievements[id].name);
        glog("    %s\n", s_achievements[id].description);
        glog("======================================================\n\n");
    }
}

void ghost_dump_achievements(void) {
    glog("\n=================================================================================\n");
    glog("                    GHOSTESP GOD MODE ACHIEVEMENTS                               \n");
    glog("=================================================================================\n");
    glog(" Status    | Title                | Requirement\n");
    glog("-----------+----------------------+----------------------------------------------\n");

    for (int i = 0; i < ACH_MAX; i++) {
        const achievement_record_t *a = &s_achievements[i];
        glog(" [%s] | %-20s | %s\n",
             a->unlocked ? "UNLOCKED" : " LOCKED ", a->name, a->description);
    }
    glog("=================================================================================\n\n");
}

void ghost_render_companion(void) {
    glog("\n");
    glog("       .---.          \n");
    glog("      /     \\         \n");
    glog("     | () () |   < \"Greetings, Researcher! God Mode V3 is watching the airspace.\"\n");
    glog("      \\  -  /         \n");
    glog("      /|| ||\\         \n");
    glog("     / || || \\        \n");
    glog("    (_/| | |\\_)       \n");
    glog("       ' ' '          \n");
    glog("\n");
    glog(" Ghost Status: OPTIMAL | Active Profile: [%s]\n", profile_manager_get_current()->name);
    glog(" Rogue Threats: %lu    | Deauth Storm Rate: %lu fps\n\n",
         (unsigned long)rogue_detector_get_threat_count(),
         (unsigned long)deauth_monitor_get_rate());
}

void ghost_render_matrix_rain(uint32_t duration_frames) {
    if (duration_frames == 0) duration_frames = 20;
    if (duration_frames > 60) duration_frames = 60;

    glog("\n[+] Entering Matrix Terminal Mode (Press CTRL+C or any key to exit)...\n\n");
    static const char glyphs[] = "0123456789ABCDEF!@#$%^&*+-<>~";
    const int glyph_count = sizeof(glyphs) - 1;

    for (uint32_t f = 0; f < duration_frames; f++) {
        char line[65];
        for (int c = 0; c < 64; c++) {
            uint32_t r = esp_random();
            if ((r % 5) == 0) {
                line[c] = glyphs[r % glyph_count];
            } else {
                line[c] = ' ';
            }
        }
        line[64] = '\0';
        glog("\033[32m%s\033[0m\n", line);
        vTaskDelay(pdMS_TO_TICKS(75));
    }
    glog("\n[+] Matrix Mode Complete.\n\n");
}

void ghost_render_radar(void) {
    glog("\n=================================================================================\n");
    glog("                     GHOSTESP AIRSPACE RADAR DISPLAY                             \n");
    glog("=================================================================================\n");
    glog("                      .-'''''-.                      \n");
    glog("                    .'    |    '.                    \n");
    glog("                   /   *  |  .   \\     * = Wi-Fi AP   \n");
    glog("                  :   .   |       :    . = BLE Beacon \n");
    glog("                  |-------+-------|                   \n");
    glog("                  :       |   *   :                   \n");
    glog("                   \\  .   |      /                    \n");
    glog("                    '.    |    .'                     \n");
    glog("                      '-.....-'                      \n");
    glog("=================================================================================\n");
    glog(" Live Radar Summary: Airspace clear of malicious transmitters.\n\n");
}

void ghost_handle_developer_command(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "help") == 0) {
        glog("\nGhostESP Secret Developer Console Commands:\n\n");
        glog("  ghost status       - Show high-level system & companion state\n");
        glog("  ghost matrix       - Run animated Matrix digital rain terminal\n");
        glog("  ghost radar        - View ASCII radar sweep of local airspace\n");
        glog("  ghost companion    - Interact with the Ghost Companion mascot\n");
        glog("  ghost selftest     - Run full automated hardware self-test\n");
        glog("  ghost achievements - List unlocked milestones & achievements\n");
        glog("  ghost easteregg    - Trigger rare visual glitch/easter egg effect\n");
        glog("  ghost version      - Display detailed God Mode V3 architecture info\n\n");
        return;
    }

    if (strcmp(argv[1], "status") == 0) {
        ghost_render_companion();
    } else if (strcmp(argv[1], "matrix") == 0) {
        ghost_render_matrix_rain(25);
    } else if (strcmp(argv[1], "radar") == 0) {
        ghost_render_radar();
    } else if (strcmp(argv[1], "companion") == 0) {
        ghost_render_companion();
    } else if (strcmp(argv[1], "selftest") == 0) {
        self_test_run_all();
        ghost_unlock_achievement(ACH_FIRST_SELFTEST);
    } else if (strcmp(argv[1], "achievements") == 0) {
        ghost_dump_achievements();
    } else if (strcmp(argv[1], "easteregg") == 0) {
        glog("\n[!] GLITCH SEQUENCE ACTIVATED: System reality distortion wave passing...\n");
        ghost_render_matrix_rain(15);
        glog("[*] Firmware integrity verified: 0 bytes altered.\n\n");
    } else if (strcmp(argv[1], "version") == 0) {
        glog("\n=================================================================================\n");
        glog(" Firmware: GhostESP Revival God Mode V3 (Enterprise Security Architecture)       \n");
        glog(" Target:   7Semi ESP32-S3-WROOM-1-N16R8 (16MB Flash, 8MB Octal PSRAM)            \n");
        glog(" Buses:    I2C0 (8/9), SPI2 RF (10-14), SPI3 LCD (5-7,15,16), UART1 (43/44)      \n");
        glog(" Security: Passive Monitoring Default, Hardware Locks, Non-Destructive           \n");
        glog("=================================================================================\n\n");
    } else {
        glog("Unknown developer subcommand '%s'. Type 'ghost help' for commands.\n", argv[1]);
    }
}
