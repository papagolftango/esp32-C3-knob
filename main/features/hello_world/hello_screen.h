#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize hello world screen
 */
void hello_screen_init(void);

/**
 * @brief Show hello world screen
 */
void hello_screen_show(void);

/**
 * @brief Update WiFi status on hello screen
 * @param connected WiFi connection status
 * @param provisioning WiFi provisioning status
 */
void hello_screen_update_wifi_status(bool connected, bool provisioning);

#ifdef __cplusplus
}
#endif
