#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_log.h>
#include <nvs_flash.h>

// Core modules
#include "core/hardware.h"
#include "core/display.h"
#include "core/wifi_manager.h"
#include "core/rotary.h"
#include "core/mqtt_manager.h"
#include "core/app_state_manager.h"

// UI management
#include "ui/screen_manager.h"

// Feature screens
#include "features/hello_world/hello_screen.h"

// MQTT integration
#include "features/energy/energy_data_manager.h"
#include "features/device_control/device_control_manager.h"
#include "features/house/bin_collection_manager.h"

static const char *TAG = "ESP32_KNOB";

// Global instances
static WiFiManager wifiManager;

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Starting ESP32-S3 Knob Touch LCD");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize core hardware
    hardware_init();
    
    // Initialize display and LVGL
    display_init();
    
    // Initialize screen manager and all feature screens
    screen_manager_init();
    
    // Set up rotary encoder callback - now handled by screen manager
    rotary_set_callback(screen_manager_rotary_event_handler);
    
    // Start LVGL task
    display_start_task();
    
    // Initialize MQTT system
    ESP_LOGI(TAG, "Initializing MQTT system...");
    MQTTManager::begin();
    EnergyDataManager::begin();
    DeviceControlManager::begin();
    BinCollectionManager::begin();
    
    // Initialize Application State Manager
    ESP_LOGI(TAG, "Initializing Application State Manager...");
    app_state_manager_begin();
    
    // Initialize WiFi Manager with state manager callbacks
    wifiManager.setCallbacks(
        app_state_wifi_connected_callback, 
        app_state_wifi_disconnected_callback, 
        app_state_provisioning_complete_callback
    );
    
    // Start WiFi initialization
    ESP_ERROR_CHECK(wifiManager.initialize());
    
    // Update initial WiFi status
    app_state_provisioning_started_callback(); // WiFi starts in provisioning mode
    
    ESP_LOGI(TAG, "ESP32-S3 Knob initialization complete");
    
    // Main application loop
    while (1) {
        // Process application state (WiFi, MQTT, UI updates)
        app_state_manager_process();
        
        // Sleep for a short time to prevent busy waiting
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
