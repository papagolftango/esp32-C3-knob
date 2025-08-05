#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize weather display screen
 */
void weather_screen_init(void);

/**
 * @brief Show weather display screen
 */
void weather_screen_show(void);

/**
 * @brief Handle rotary encoder button press on weather screen
 */
void weather_screen_handle_button_press(void);

#ifdef __cplusplus
}
#endif
