#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize smart home control screen
 */
void house_screen_init(void);

/**
 * @brief Show smart home control screen
 */
void house_screen_show(void);

/**
 * @brief Handle rotary encoder button press on house screen
 */
void house_screen_handle_button_press(void);

#ifdef __cplusplus
}
#endif
