#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize settings screen
 */
void settings_screen_init(void);

/**
 * @brief Show settings screen
 */
void settings_screen_show(void);

/**
 * @brief Handle rotary encoder button press on settings screen
 */
void settings_screen_handle_button_press(void);

/**
 * @brief Handle rotary encoder rotation on settings screen
 * @param direction Rotation direction: 1 for clockwise, -1 for counter-clockwise
 */
void settings_screen_handle_rotation(int direction);

/**
 * @brief Cycle through brightness levels (25%, 50%, 75%, 100%)
 */
void settings_screen_cycle_brightness(void);

/**
 * @brief Toggle haptic feedback on/off
 */
void settings_screen_toggle_haptic(void);

#ifdef __cplusplus
}
#endif
