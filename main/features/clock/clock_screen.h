#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize clock screen
 */
void clock_screen_init(void);

/**
 * @brief Show clock screen
 */
void clock_screen_show(void);

/**
 * @brief Update clock display with current time
 */
void clock_screen_update_time(void);

/**
 * @brief Handle rotary encoder button press on clock screen
 */
void clock_screen_handle_button_press(void);

#ifdef __cplusplus
}
#endif
