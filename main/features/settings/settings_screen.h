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

#ifdef __cplusplus
}
#endif
