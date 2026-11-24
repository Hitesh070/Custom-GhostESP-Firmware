#ifndef UNIVERSAL_IR_H
#define UNIVERSAL_IR_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "managers/infrared_manager.h"

typedef enum {
    UNIVERSAL_IR_ACTION_POWER = 0,
    UNIVERSAL_IR_ACTION_MUTE,
    UNIVERSAL_IR_ACTION_VOL_UP,
    UNIVERSAL_IR_ACTION_VOL_DOWN,
    UNIVERSAL_IR_ACTION_CH_UP,
    UNIVERSAL_IR_ACTION_CH_DOWN,
    UNIVERSAL_IR_ACTION_INPUT,
    UNIVERSAL_IR_ACTION_OTHER
} universal_ir_action_t;

typedef struct {
    const char *name;
    const char *protocol;
    uint32_t address;
    uint32_t command;
    universal_ir_action_t action;
} universal_ir_signal_t;

// Extern signal array definition located in universal_ir.c (saves flash/RAM deduplication)
extern const universal_ir_signal_t universal_ir_signals[];

size_t universal_ir_get_signal_count(void);
#define UNIVERSAL_IR_SIGNAL_COUNT universal_ir_get_signal_count()

bool universal_ir_get_signal(size_t index, infrared_signal_t *signal);
const char* universal_ir_get_signal_name(size_t index);
universal_ir_action_t universal_ir_get_signal_action(size_t index);
bool universal_ir_matches_action(size_t index, const char *action_query);
const char* universal_ir_action_to_string(universal_ir_action_t action);
size_t universal_ir_count_by_action(universal_ir_action_t action);

#endif // UNIVERSAL_IR_H
