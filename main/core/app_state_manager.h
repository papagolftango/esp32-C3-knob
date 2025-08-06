#pragma once

#include <esp_log.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Application State Manager
 * 
 * Centralized management of application state including WiFi, MQTT,
 * and provisioning status. Handles state transitions and coordinates
 * between different subsystems (UI updates, MQTT connections, etc.)
 */

// Application state structure
typedef struct {
    bool wifi_connected;
    bool wifi_provisioning;
    bool mqtt_connected;
    unsigned long last_wifi_connect_time;
    unsigned long last_mqtt_connect_time;
} app_state_t;

/**
 * @brief Initialize the application state manager
 */
void app_state_manager_begin(void);

/**
 * @brief Initialize all feature managers (data managers)
 */
void app_state_init_feature_managers(void);

/**
 * @brief Process state management tasks (call in main loop)
 */
void app_state_manager_process(void);

/**
 * @brief Get current application state
 * @return Pointer to current application state (read-only)
 */
const app_state_t* app_state_manager_get_state(void);

// WiFi callback functions for use with WiFiManager
void app_state_wifi_connected_callback(void);
void app_state_wifi_disconnected_callback(void);
void app_state_provisioning_complete_callback(void);
void app_state_provisioning_started_callback(void);

// State query functions (convenience methods)
bool app_state_is_wifi_connected(void);
bool app_state_is_mqtt_connected(void);
bool app_state_is_provisioning(void);

// Manual state updates (for when state changes outside of callbacks)
void app_state_set_mqtt_connected(bool connected);

#ifdef __cplusplus
}
#endif
