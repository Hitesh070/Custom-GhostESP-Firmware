#include "core/universal_ir.h"
#include <string.h>
#include <strings.h>
#include <ctype.h>

const universal_ir_signal_t universal_ir_signals[] = {
    // --- POWER SIGNALS (Original GhostESP verified universal power database) ---
    {"POWER", "NEC", 0x00000004, 0x00000008, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "Samsung32", 0x00000007, 0x00000002, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "SIRC", 0x00000001, 0x00000015, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "Kaseikyo", 0x00200280, 0x000003D0, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00007D02, 0x0000B946, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "RC5", 0x00000000, 0x0000000C, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "RC5", 0x00000001, 0x0000000C, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "RC6", 0x00000000, 0x0000000C, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "Samsung32", 0x00000007, 0x000000E6, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000040, 0x00000012, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "RCA", 0x0000000F, 0x00000054, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000BF00, 0x0000F20D, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00000586, 0x0000F00F, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000C7EA, 0x0000E817, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000020, 0x00000052, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00007F00, 0x0000F50A, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000001, 0x00000010, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "RC5", 0x00000003, 0x0000000C, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x000000A0, 0x0000005F, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00007F00, 0x0000EA15, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00007F00, 0x0000E11E, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00007201, 0x0000E11E, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000DD72, 0x0000F10E, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000E084, 0x0000DF20, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000040, 0x0000000B, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x0000006E, 0x00000002, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000DF00, 0x0000E31C, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00004040, 0x0000F50A, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "Samsung32", 0x0000000E, 0x0000000C, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000000, 0x0000001A, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000001, 0x0000001C, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000001, 0x00000040, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000004, 0x00000040, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000008, 0x000000D7, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000019, 0x00000018, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000028, 0x0000000B, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000038, 0x00000001, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000038, 0x00000012, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000050, 0x00000017, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000080, 0x00000012, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x00000080, 0x00000082, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NEC", 0x000000A0, 0x0000001C, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000BD00, 0x0000FE01, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000BF00, 0x0000FF00, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000BF00, 0x0000FC03, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000F700, 0x0000F30C, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000FB00, 0x0000F50A, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00003E01, 0x0000F50A, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000B904, 0x0000FF00, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000F404, 0x0000F708, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00004664, 0x0000A25D, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00006969, 0x0000FE01, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000DD72, 0x0000EF10, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00007A83, 0x00000008, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x00007C85, 0x00007F80, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000B7A0, 0x000016E9, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000EDAD, 0x00004AB5, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "NECext", 0x0000C7EA, 0x00006897, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "Pioneer", 0x000000AA, 0x0000001C, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "SIRC", 0x00000001, 0x0000002E, UNIVERSAL_IR_ACTION_POWER},
    {"POWER", "Samsung32", 0x0000003E, 0x0000000C, UNIVERSAL_IR_ACTION_POWER},

    // --- MUTE SIGNALS (Universal TV/Audio Mute) ---
    {"MUTE", "NEC", 0x00000004, 0x00000009, UNIVERSAL_IR_ACTION_MUTE},
    {"MUTE", "Samsung32", 0x00000007, 0x0000000F, UNIVERSAL_IR_ACTION_MUTE},
    {"MUTE", "SIRC", 0x00000001, 0x00000014, UNIVERSAL_IR_ACTION_MUTE},
    {"MUTE", "RC5", 0x00000000, 0x0000000D, UNIVERSAL_IR_ACTION_MUTE},
    {"MUTE", "RC6", 0x00000000, 0x0000000D, UNIVERSAL_IR_ACTION_MUTE},
    {"MUTE", "NECext", 0x0000BF00, 0x0000F30C, UNIVERSAL_IR_ACTION_MUTE},
    {"MUTE", "Kaseikyo", 0x00200280, 0x000003D1, UNIVERSAL_IR_ACTION_MUTE},
    {"MUTE", "NEC", 0x00000040, 0x00000009, UNIVERSAL_IR_ACTION_MUTE},

    // --- VOLUME UP SIGNALS ---
    {"VOL+", "NEC", 0x00000004, 0x00000002, UNIVERSAL_IR_ACTION_VOL_UP},
    {"VOL+", "Samsung32", 0x00000007, 0x00000007, UNIVERSAL_IR_ACTION_VOL_UP},
    {"VOL+", "SIRC", 0x00000001, 0x00000012, UNIVERSAL_IR_ACTION_VOL_UP},
    {"VOL+", "RC5", 0x00000000, 0x00000010, UNIVERSAL_IR_ACTION_VOL_UP},
    {"VOL+", "RC6", 0x00000000, 0x00000010, UNIVERSAL_IR_ACTION_VOL_UP},
    {"VOL+", "NECext", 0x0000BF00, 0x0000F40B, UNIVERSAL_IR_ACTION_VOL_UP},

    // --- VOLUME DOWN SIGNALS ---
    {"VOL-", "NEC", 0x00000004, 0x00000003, UNIVERSAL_IR_ACTION_VOL_DOWN},
    {"VOL-", "Samsung32", 0x00000007, 0x0000000B, UNIVERSAL_IR_ACTION_VOL_DOWN},
    {"VOL-", "SIRC", 0x00000001, 0x00000013, UNIVERSAL_IR_ACTION_VOL_DOWN},
    {"VOL-", "RC5", 0x00000000, 0x00000011, UNIVERSAL_IR_ACTION_VOL_DOWN},
    {"VOL-", "RC6", 0x00000000, 0x00000011, UNIVERSAL_IR_ACTION_VOL_DOWN},
    {"VOL-", "NECext", 0x0000BF00, 0x0000F50A, UNIVERSAL_IR_ACTION_VOL_DOWN},

    // --- CHANNEL UP SIGNALS ---
    {"CH+", "NEC", 0x00000004, 0x00000000, UNIVERSAL_IR_ACTION_CH_UP},
    {"CH+", "Samsung32", 0x00000007, 0x00000012, UNIVERSAL_IR_ACTION_CH_UP},
    {"CH+", "SIRC", 0x00000001, 0x00000010, UNIVERSAL_IR_ACTION_CH_UP},
    {"CH+", "RC5", 0x00000000, 0x00000020, UNIVERSAL_IR_ACTION_CH_UP},

    // --- CHANNEL DOWN SIGNALS ---
    {"CH-", "NEC", 0x00000004, 0x00000001, UNIVERSAL_IR_ACTION_CH_DOWN},
    {"CH-", "Samsung32", 0x00000007, 0x00000010, UNIVERSAL_IR_ACTION_CH_DOWN},
    {"CH-", "SIRC", 0x00000001, 0x00000011, UNIVERSAL_IR_ACTION_CH_DOWN},
    {"CH-", "RC5", 0x00000000, 0x00000021, UNIVERSAL_IR_ACTION_CH_DOWN},

    // --- INPUT / SOURCE SIGNALS ---
    {"INPUT", "NEC", 0x00000004, 0x0000000B, UNIVERSAL_IR_ACTION_INPUT},
    {"INPUT", "Samsung32", 0x00000007, 0x00000001, UNIVERSAL_IR_ACTION_INPUT},
    {"INPUT", "SIRC", 0x00000001, 0x00000025, UNIVERSAL_IR_ACTION_INPUT},
    {"INPUT", "RC5", 0x00000000, 0x00000038, UNIVERSAL_IR_ACTION_INPUT},
};

static const size_t s_universal_ir_signal_count = sizeof(universal_ir_signals) / sizeof(universal_ir_signals[0]);

size_t universal_ir_get_signal_count(void) {
    return s_universal_ir_signal_count;
}

bool universal_ir_get_signal(size_t index, infrared_signal_t *signal) {
    if (!signal || index >= s_universal_ir_signal_count) {
        return false;
    }
    
    const universal_ir_signal_t *uni_signal = &universal_ir_signals[index];
    
    memset(signal, 0, sizeof(infrared_signal_t));
    strncpy(signal->name, uni_signal->name, sizeof(signal->name) - 1);
    signal->name[sizeof(signal->name) - 1] = '\0';
    
    signal->is_raw = false;
    strncpy(signal->payload.message.protocol, uni_signal->protocol, sizeof(signal->payload.message.protocol) - 1);
    signal->payload.message.protocol[sizeof(signal->payload.message.protocol) - 1] = '\0';
    signal->payload.message.address = uni_signal->address;
    signal->payload.message.command = uni_signal->command;
    
    return true;
}

const char* universal_ir_get_signal_name(size_t index) {
    if (index >= s_universal_ir_signal_count) {
        return NULL;
    }
    return universal_ir_signals[index].name;
}

universal_ir_action_t universal_ir_get_signal_action(size_t index) {
    if (index >= s_universal_ir_signal_count) {
        return UNIVERSAL_IR_ACTION_OTHER;
    }
    return universal_ir_signals[index].action;
}

const char* universal_ir_action_to_string(universal_ir_action_t action) {
    switch (action) {
        case UNIVERSAL_IR_ACTION_POWER:    return "POWER";
        case UNIVERSAL_IR_ACTION_MUTE:     return "MUTE";
        case UNIVERSAL_IR_ACTION_VOL_UP:   return "VOL+";
        case UNIVERSAL_IR_ACTION_VOL_DOWN: return "VOL-";
        case UNIVERSAL_IR_ACTION_CH_UP:    return "CH+";
        case UNIVERSAL_IR_ACTION_CH_DOWN:  return "CH-";
        case UNIVERSAL_IR_ACTION_INPUT:    return "INPUT";
        default:                           return "OTHER";
    }
}

size_t universal_ir_count_by_action(universal_ir_action_t action) {
    size_t count = 0;
    for (size_t i = 0; i < s_universal_ir_signal_count; i++) {
        if (universal_ir_signals[i].action == action) {
            count++;
        }
    }
    return count;
}

bool universal_ir_matches_action(size_t index, const char *action_query) {
    if (index >= s_universal_ir_signal_count || !action_query || !*action_query) {
        return false;
    }

    const universal_ir_signal_t *sig = &universal_ir_signals[index];

    // Check action category aliases
    if (strcasecmp(action_query, "power") == 0 ||
        strcasecmp(action_query, "power off") == 0 ||
        strcasecmp(action_query, "power_off") == 0 ||
        strcasecmp(action_query, "poweron") == 0 ||
        strcasecmp(action_query, "pwr") == 0 ||
        strcasecmp(action_query, "turnhistvoff") == 0) {
        return (sig->action == UNIVERSAL_IR_ACTION_POWER);
    }

    if (strcasecmp(action_query, "mute") == 0) {
        return (sig->action == UNIVERSAL_IR_ACTION_MUTE);
    }

    if (strcasecmp(action_query, "vol+") == 0 ||
        strcasecmp(action_query, "vol_up") == 0 ||
        strcasecmp(action_query, "vol up") == 0 ||
        strcasecmp(action_query, "volume_up") == 0 ||
        strcasecmp(action_query, "volume up") == 0) {
        return (sig->action == UNIVERSAL_IR_ACTION_VOL_UP);
    }

    if (strcasecmp(action_query, "vol-") == 0 ||
        strcasecmp(action_query, "vol_down") == 0 ||
        strcasecmp(action_query, "vol down") == 0 ||
        strcasecmp(action_query, "volume_down") == 0 ||
        strcasecmp(action_query, "volume down") == 0) {
        return (sig->action == UNIVERSAL_IR_ACTION_VOL_DOWN);
    }

    if (strcasecmp(action_query, "ch+") == 0 ||
        strcasecmp(action_query, "ch_up") == 0 ||
        strcasecmp(action_query, "ch up") == 0 ||
        strcasecmp(action_query, "channel_up") == 0 ||
        strcasecmp(action_query, "channel up") == 0) {
        return (sig->action == UNIVERSAL_IR_ACTION_CH_UP);
    }

    if (strcasecmp(action_query, "ch-") == 0 ||
        strcasecmp(action_query, "ch_down") == 0 ||
        strcasecmp(action_query, "ch down") == 0 ||
        strcasecmp(action_query, "channel_down") == 0 ||
        strcasecmp(action_query, "channel down") == 0) {
        return (sig->action == UNIVERSAL_IR_ACTION_CH_DOWN);
    }

    if (strcasecmp(action_query, "input") == 0 ||
        strcasecmp(action_query, "source") == 0 ||
        strcasecmp(action_query, "av") == 0) {
        return (sig->action == UNIVERSAL_IR_ACTION_INPUT);
    }

    // Direct case-insensitive string match
    return (strcasecmp(sig->name, action_query) == 0);
}
