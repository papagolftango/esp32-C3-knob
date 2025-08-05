#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize energy monitoring screen
 */
void energy_screen_init(void);

/**
 * @brief Show energy monitoring screen
 */
void energy_screen_show(void);

/**
 * @brief Handle rotary encoder button press on energy screen
 */
void energy_screen_handle_button_press(void);

#ifdef __cplusplus
}
#endif
