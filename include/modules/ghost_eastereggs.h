#ifndef MODULES_GHOST_EASTEREGGS_H
#define MODULES_GHOST_EASTEREGGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    ACH_FIRST_BOOT = 0,
    ACH_FIRST_WIFI_SCAN,
    ACH_FIRST_BLE_SCAN,
    ACH_FIRST_NRF24_PROBE,
    ACH_FIRST_SELFTEST,
    ACH_100_DEVICES,
    ACH_MAX
} achievement_id_t;

/**
 * @brief Initialize the Easter Egg, Matrix & Achievement System.
 */
esp_err_t ghost_eastereggs_init(void);

/**
 * @brief Render the Matrix digital rain animation in console.
 *
 * @param duration_frames Number of animation frames to render.
 */
void ghost_render_matrix_rain(uint32_t duration_frames);

/**
 * @brief Render the Ghost Radar visualizer in console.
 */
void ghost_render_radar(void);

/**
 * @brief Render the Ghost Companion character and diagnostic mood.
 */
void ghost_render_companion(void);

/**
 * @brief Unlock a genuine milestone achievement.
 *
 * @param id Achievement ID.
 */
void ghost_unlock_achievement(achievement_id_t id);

/**
 * @brief Dump all achievements and unlocked status.
 */
void ghost_dump_achievements(void);

/**
 * @brief Handle developer console CLI command: 'ghost <subcommand>'
 */
void ghost_handle_developer_command(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* MODULES_GHOST_EASTEREGGS_H */
