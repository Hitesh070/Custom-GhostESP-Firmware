#include "modules/rogue_detector.h"
#include "modules/deauth_monitor.h"
#include "modules/nrf24_lab.h"
#include "modules/profile_manager.h"
#include "modules/self_test.h"
#include "modules/ghost_eastereggs.h"
#include "modules/temp_monitor.h"
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

void handle_ghost_cmd(int argc, char **argv) {
    ghost_handle_developer_command(argc, argv);
}

void handle_temp_cmd(int argc, char **argv) {
    if (argc < 2) {
        temp_monitor_print_report(false);
        return;
    }
    const char *sub = argv[1];
    if (strcmp(sub, "c") == 0 || strcmp(sub, "C") == 0) {
        temp_monitor_set_unit(TEMP_UNIT_CELSIUS);
        temp_monitor_print_report(false);
    } else if (strcmp(sub, "f") == 0 || strcmp(sub, "F") == 0) {
        temp_monitor_set_unit(TEMP_UNIT_FAHRENHEIT);
        temp_monitor_print_report(false);
    } else if (strcmp(sub, "json") == 0) {
        temp_monitor_print_report(true);
    } else if (strcmp(sub, "status") == 0) {
        temp_monitor_print_report(false);
    } else if (strcmp(sub, "history") == 0) {
        temp_monitor_print_history();
    } else if (strcmp(sub, "reset") == 0) {
        temp_monitor_reset_stats();
        glog("[+] Temperature monitor statistics & history reset.\n");
    } else if (strcmp(sub, "interval") == 0) {
        if (argc < 3) {
            glog("Usage: temp interval <ms> (500 - 60000)\n");
            return;
        }
        uint32_t ms = (uint32_t)strtoul(argv[2], NULL, 10);
        esp_err_t err = temp_monitor_set_interval_ms(ms);
        if (err != ESP_OK) {
            glog("[-] Invalid interval %lu ms (allowed: 500 - 60000 ms)\n", (unsigned long)ms);
        } else {
            glog("[+] Temperature monitor sampling interval set to %lu ms\n", (unsigned long)ms);
        }
    } else {
        glog("Usage: temp [status|c|f|interval <ms>|history|reset|json]\n");
    }
}
