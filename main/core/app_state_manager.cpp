#include "app_state_manager.h"
#include "mqtt_manager.h"
#include "../features/hello_world/hello_screen.h"
#include "../features/device_control/device_control_manager.h"

static const char *TAG = "APP_STATE";

// Application state
static app_state_t app_state = {
    .wifi_connected = false,
    .wifi_provisioning = false,
    .mqtt_connected = false,
    .last_wifi_connect_time = 0,
    .last_mqtt_connect_time = 0
};

void app_state_manager_begin(void) {
    ESP_LOGI(TAG, "Initializing Application State Manager");
    
    // Initialize state to default values
    app_state.wifi_connected = false;
    app_state.wifi_provisioning = false;
    app_state.mqtt_connected = false;
    app_state.last_wifi_connect_time = 0;
    app_state.last_mqtt_connect_time = 0;
    
    ESP_LOGI(TAG, "Application State Manager initialized");
}

void app_state_manager_process(void) {
    // Handle MQTT state monitoring and reconnection
    if (app_state.wifi_connected && !app_state.mqtt_connected) {
        // Check if we should attempt MQTT reconnection
        unsigned long current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        // Attempt reconnection every 30 seconds if disconnected
        if (current_time - app_state.last_mqtt_connect_time > 30000) {
            ESP_LOGI(TAG, "Attempting MQTT reconnection...");
            if (MQTTManager::connect()) {
                app_state.mqtt_connected = true;
                app_state.last_mqtt_connect_time = current_time;
                ESP_LOGI(TAG, "MQTT reconnected successfully");
                
                // Publish status after reconnection
                DeviceControlManager::publishStatus();
            } else {
                app_state.last_mqtt_connect_time = current_time;
                ESP_LOGW(TAG, "MQTT reconnection failed, will retry in 30 seconds");
            }
        }
    }
    
    // Monitor MQTT status changes
    if (app_state.wifi_connected && MQTTManager::hasStatusChanged()) {
        bool current_mqtt_status = MQTTManager::isConnected();
        if (current_mqtt_status != app_state.mqtt_connected) {
            app_state.mqtt_connected = current_mqtt_status;
            ESP_LOGI(TAG, "MQTT status changed: %s", 
                     app_state.mqtt_connected ? "Connected" : "Disconnected");
            
            if (app_state.mqtt_connected) {
                app_state.last_mqtt_connect_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
                DeviceControlManager::publishStatus();
            }
        }
    }
}

const app_state_t* app_state_manager_get_state(void) {
    return &app_state;
}

void app_state_wifi_connected_callback(void) {
    ESP_LOGI(TAG, "WiFi connected successfully");
    
    // Update state
    app_state.wifi_connected = true;
    app_state.wifi_provisioning = false;
    app_state.last_wifi_connect_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Update UI
    hello_screen_update_wifi_status(app_state.wifi_connected, app_state.wifi_provisioning);
    
    // Attempt MQTT connection
    ESP_LOGI(TAG, "Attempting MQTT connection...");
    if (MQTTManager::connect()) {
        ESP_LOGI(TAG, "MQTT connected successfully");
        app_state.mqtt_connected = true;
        app_state.last_mqtt_connect_time = app_state.last_wifi_connect_time;
        
        // Publish initial device status
        DeviceControlManager::publishStatus();
    } else {
        ESP_LOGW(TAG, "MQTT connection failed, will retry automatically");
        app_state.mqtt_connected = false;
    }
}

void app_state_wifi_disconnected_callback(void) {
    ESP_LOGI(TAG, "WiFi disconnected");
    
    // Update state
    app_state.wifi_connected = false;
    app_state.mqtt_connected = false;
    
    // Update UI
    hello_screen_update_wifi_status(app_state.wifi_connected, app_state.wifi_provisioning);
}

void app_state_provisioning_complete_callback(void) {
    ESP_LOGI(TAG, "WiFi provisioning completed");
    
    // Update state
    app_state.wifi_provisioning = false;
    
    // Update UI
    hello_screen_update_wifi_status(app_state.wifi_connected, app_state.wifi_provisioning);
}

void app_state_provisioning_started_callback(void) {
    ESP_LOGI(TAG, "WiFi provisioning started");
    
    // Update state
    app_state.wifi_provisioning = true;
    
    // Update UI
    hello_screen_update_wifi_status(app_state.wifi_connected, app_state.wifi_provisioning);
}

// Convenience query functions
bool app_state_is_wifi_connected(void) {
    return app_state.wifi_connected;
}

bool app_state_is_mqtt_connected(void) {
    return app_state.mqtt_connected;
}

bool app_state_is_provisioning(void) {
    return app_state.wifi_provisioning;
}

void app_state_set_mqtt_connected(bool connected) {
    if (app_state.mqtt_connected != connected) {
        app_state.mqtt_connected = connected;
        ESP_LOGI(TAG, "MQTT status manually updated: %s", connected ? "Connected" : "Disconnected");
        
        if (connected) {
            app_state.last_mqtt_connect_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        }
    }
}
