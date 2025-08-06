#pragma once

#include "lvgl.h"
#include "../core/rotary.h"  // For rotary_event_t

typedef enum {
    SCREEN_HELLO_WORLD = 0,
    SCREEN_SETTINGS,
    SCREEN_CLOCK,       // 🆕 New clock screen
    SCREEN_ENERGY,      // 🆕 Energy monitoring
    SCREEN_WEATHER,     // 🆕 Weather display
    SCREEN_HOUSE,       // 🆕 Smart home control
    SCREEN_COUNT  // Keep this last
} screen_id_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize screen manager
 */
void screen_manager_init(void);

/**
 * @brief Switch to specified screen
 * @param screen_id The screen to switch to
 */
void screen_manager_switch_to(screen_id_t screen_id);

/**
 * @brief Navigate to next screen (rotary clockwise)
 */
void screen_manager_next(void);

/**
 * @brief Navigate to previous screen (rotary counter-clockwise)
 */
void screen_manager_previous(void);

/**
 * @brief Get current screen ID
 * @return Current screen ID
 */
screen_id_t screen_manager_get_current(void);

/**
 * @brief Handle rotary encoder rotation
 * @param direction 1 for clockwise, -1 for counter-clockwise
 */
void screen_manager_handle_rotation(int direction);

/**
 * @brief Handle rotary encoder button press
 */
void screen_manager_handle_button_press(void);

/**
 * @brief Rotary encoder event handler (for use as callback)
 * @param event The rotary encoder event
 */
void screen_manager_rotary_event_handler(rotary_event_t event);

#ifdef __cplusplus
}
#endif
