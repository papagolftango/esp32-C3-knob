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

// UI management
#include "ui/screen_manager.h"

// Feature screens
#include "features/hello_world/hello_screen.h"

// MQTT integration
#include "features/energy/energy_data_manager.h"
#include "features/device_control/device_control_manager.h"

static const char *TAG = "ESP32_KNOB";

// Rotary encoder diagnostic function
void diagnose_rotary_hardware(void) {
    ESP_LOGI(TAG, "=== ROTARY ENCODER HARDWARE DIAGNOSTIC ===");
    
    // Configure all rotary pins as inputs with pullups
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << ENCODER_A_PIN) | (1ULL << ENCODER_B_PIN) | (1ULL << ENCODER_BTN_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&config);
    
    ESP_LOGI(TAG, "Encoder A: GPIO_%d", ENCODER_A_PIN);
    ESP_LOGI(TAG, "Encoder B: GPIO_%d", ENCODER_B_PIN);  
    ESP_LOGI(TAG, "Button:    GPIO_%d", ENCODER_BTN_PIN);
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Testing for 15 seconds...");
    ESP_LOGI(TAG, "Try: 1) Turn knob clockwise/counter-clockwise");
    ESP_LOGI(TAG, "Try: 2) Push knob down (if button exists)");
    ESP_LOGI(TAG, "");
    
    int last_a = -1, last_b = -1, last_btn = -1;
    int a_changes = 0, b_changes = 0, btn_changes = 0;
    
    for (int i = 0; i < 150; i++) {  // 15 seconds
        int a = gpio_get_level(ENCODER_A_PIN);
        int b = gpio_get_level(ENCODER_B_PIN);
        int btn = gpio_get_level(ENCODER_BTN_PIN);
        
        // Track A channel changes
        if (a != last_a && last_a != -1) {
            a_changes++;
            ESP_LOGI(TAG, "🔴 A-Switch: %d→%d (change #%d)", last_a, a, a_changes);
        }
        
        // Track B channel changes  
        if (b != last_b && last_b != -1) {
            b_changes++;
            ESP_LOGI(TAG, "� B-Switch: %d→%d (change #%d)", last_b, b, b_changes);
        }
        
        // Track button changes
        if (btn != last_btn && last_btn != -1) {
            btn_changes++;
            if (btn == 0) {
                ESP_LOGI(TAG, "� BUTTON PRESS! (change #%d)", btn_changes);
            } else {
                ESP_LOGI(TAG, "🟡 BUTTON RELEASE! (change #%d)", btn_changes);
            }
        }
        
        last_a = a;
        last_b = b; 
        last_btn = btn;
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    ESP_LOGI(TAG, "=== DIAGNOSTIC RESULTS ===");
    ESP_LOGI(TAG, "A-Switch changes: %d", a_changes);
    ESP_LOGI(TAG, "B-Switch changes: %d", b_changes);
    ESP_LOGI(TAG, "Button changes:   %d", btn_changes);
    ESP_LOGI(TAG, "");
    
    if (a_changes > 0 && b_changes > 0) {
        ESP_LOGI(TAG, "✅ ROTARY ENCODER DETECTED - Both A & B switches working");
    } else if (a_changes > 0 || b_changes > 0) {
        ESP_LOGW(TAG, "⚠️  PARTIAL ROTARY - Only one switch working");
    } else {
        ESP_LOGW(TAG, "❌ NO ROTARY DETECTED - No switch activity");
    }
    
    if (btn_changes > 0) {
        ESP_LOGI(TAG, "✅ PUSH BUTTON DETECTED - Button is working");
    } else {
        ESP_LOGW(TAG, "❌ NO BUTTON DETECTED - No button activity");
        ESP_LOGW(TAG, "   This could mean:");
        ESP_LOGW(TAG, "   • No button on this encoder");
        ESP_LOGW(TAG, "   • Button wired to different GPIO");
        ESP_LOGW(TAG, "   • Button not pressed during test");
    }
    ESP_LOGI(TAG, "========================");
}

// Global instances
static WiFiManager wifiManager;

// WiFi status tracking
static bool wifi_connected = false;
static bool wifi_provisioning = false;

// WiFi callback functions
void wifi_connected_callback(void) {
    ESP_LOGI(TAG, "WiFi connected successfully");
    wifi_connected = true;
    wifi_provisioning = false;
    
    // Update hello screen with new WiFi status
    hello_screen_update_wifi_status(wifi_connected, wifi_provisioning);
}

void wifi_disconnected_callback(void) {
    ESP_LOGI(TAG, "WiFi disconnected");
    wifi_connected = false;
    
    // Update hello screen with new WiFi status
    hello_screen_update_wifi_status(wifi_connected, wifi_provisioning);
}

void provisioning_complete_callback(void) {
    ESP_LOGI(TAG, "WiFi provisioning completed");
    wifi_provisioning = false;
    
    // Update hello screen with new WiFi status
    hello_screen_update_wifi_status(wifi_connected, wifi_provisioning);
}

void provisioning_started_callback(void) {
    ESP_LOGI(TAG, "WiFi provisioning started");
    wifi_provisioning = true;
    
    // Update hello screen with new WiFi status
    hello_screen_update_wifi_status(wifi_connected, wifi_provisioning);
}

// Rotary encoder event handler
void rotary_event_handler(rotary_event_t event) {
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
    
    // Set up rotary encoder callback
    rotary_set_callback(rotary_event_handler);
    
    // Start LVGL task
    display_start_task();
    
    // Initialize WiFi Manager with callbacks
    wifiManager.setCallbacks(wifi_connected_callback, wifi_disconnected_callback, provisioning_complete_callback);
    
    // Start WiFi initialization
    ESP_ERROR_CHECK(wifiManager.initialize());
    
    // Update initial WiFi status
    provisioning_started_callback(); // WiFi starts in provisioning mode
    
    ESP_LOGI(TAG, "ESP32-S3 Knob initialization complete");
    
    // Main application loop - now just wait since rotary events drive navigation
    while (1) {
        // Process any additional background tasks here
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
