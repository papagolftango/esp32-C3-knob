#include "screen_manager.h"
#include "../features/hello_world/hello_screen.h"
#include "../features/settings/settings_screen.h"
#include "../features/clock/clock_screen.h"
#include "../features/energy/energy_screen.h"
#include "../features/weather/weather_screen.h"
#include "../features/house/house_screen.h"
#include <esp_log.h>

static const char *TAG = "SCREEN_MGR";

static screen_id_t current_screen = SCREEN_HELLO_WORLD;

void screen_manager_init(void) {
    ESP_LOGI(TAG, "Initializing screen manager");
    
    // Initialize all feature screens
    hello_screen_init();
    settings_screen_init();
    clock_screen_init();
    energy_screen_init();
    weather_screen_init();
    house_screen_init();
    
    // Start with hello world screen
    screen_manager_switch_to(SCREEN_HELLO_WORLD);
    
    ESP_LOGI(TAG, "Screen manager initialized");
}

void screen_manager_switch_to(screen_id_t screen_id) {
    if (screen_id >= SCREEN_COUNT) {
        ESP_LOGW(TAG, "Invalid screen ID: %d", screen_id);
        return;
    }
    
    ESP_LOGI(TAG, "Switching to screen: %d", screen_id);
    
    current_screen = screen_id;
    
    switch (screen_id) {
        case SCREEN_HELLO_WORLD:
            hello_screen_show();
            break;
        case SCREEN_SETTINGS:
            settings_screen_show();
            break;
        case SCREEN_CLOCK:
            clock_screen_show();
            break;
        case SCREEN_ENERGY:
            energy_screen_show();
            break;
        case SCREEN_WEATHER:
            weather_screen_show();
            break;
        case SCREEN_HOUSE:
            house_screen_show();
            break;
        default:
            ESP_LOGW(TAG, "Unhandled screen ID: %d", screen_id);
            break;
    }
}

void screen_manager_next(void) {
    screen_id_t next_screen = (screen_id_t)((current_screen + 1) % SCREEN_COUNT);
    screen_manager_switch_to(next_screen);
}

void screen_manager_previous(void) {
    screen_id_t prev_screen = (screen_id_t)((current_screen + SCREEN_COUNT - 1) % SCREEN_COUNT);
    screen_manager_switch_to(prev_screen);
}

screen_id_t screen_manager_get_current(void) {
    return current_screen;
}

void screen_manager_handle_rotation(int direction) {
    if (direction > 0) {
        screen_manager_next();
    } else if (direction < 0) {
        screen_manager_previous();
    }
}

void screen_manager_handle_button_press(void) {
    ESP_LOGI(TAG, "Button press on screen: %d", current_screen);
    
    // Delegate button press to current screen
    switch (current_screen) {
        case SCREEN_HELLO_WORLD:
            // Hello world screen doesn't handle button presses currently
            break;
        case SCREEN_SETTINGS:
            settings_screen_handle_button_press();
            break;
        case SCREEN_CLOCK:
            clock_screen_handle_button_press();
            break;
        case SCREEN_ENERGY:
            energy_screen_handle_button_press();
            break;
        case SCREEN_WEATHER:
            weather_screen_handle_button_press();
            break;
        case SCREEN_HOUSE:
            house_screen_handle_button_press();
            break;
        default:
            ESP_LOGW(TAG, "Unhandled button press for screen: %d", current_screen);
            break;
    }
}

void screen_manager_rotary_event_handler(rotary_event_t event) {
    ESP_LOGI(TAG, "Rotary event: %d", event);
    
    switch (event) {
        case ROTARY_EVENT_CW:
            screen_manager_next();
            break;
        case ROTARY_EVENT_CCW:
            screen_manager_previous();
            break;
        case ROTARY_EVENT_BUTTON:
            screen_manager_handle_button_press();
            break;
        default:
            break;
    }
}
