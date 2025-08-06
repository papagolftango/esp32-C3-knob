#include "device_control_manager.h"
#include "../../core/display.h"  // For brightness control
#include "../settings/settings_screen.h"  // For settings integration
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_netif.h>
#include <esp_heap_caps.h>
#include <string>
#include <cstring>

static const char *TAG = "DEVICE_CONTROL";

// Static member definitions
int DeviceControlManager::current_brightness = 75;  // Default 75%
bool DeviceControlManager::haptic_enabled = true;   // Default enabled

void DeviceControlManager::begin() {
    ESP_LOGI(TAG, "Initializing Device Control Manager");
    
    // Register our handler with the MQTT manager for home/knob namespace
    MQTTManager::registerHandler("home/knob", handleMQTTMessage);
    
    // Publish initial status
    publishStatus();
    
    ESP_LOGI(TAG, "Device Control Manager initialized - registered for home/knob/* topics");
}

void DeviceControlManager::handleMQTTMessage(const char* topic, const char* message) {
    ESP_LOGI(TAG, "Processing device control message: %s = %s", topic, message);
    
    if (strcmp(topic, "home/knob/command") == 0) {
        processDeviceCommand(message);
    } else if (strcmp(topic, "home/knob/brightness") == 0) {
        processBrightnessCommand(message);
    } else if (strcmp(topic, "home/knob/haptic") == 0) {
        processHapticCommand(message);
    } else {
        ESP_LOGW(TAG, "Unknown device control topic: %s", topic);
    }
}

void DeviceControlManager::processBrightnessCommand(const char* message) {
    int brightness = atoi(message);
    
    // Validate range (0-100%)
    if (brightness < 0 || brightness > 100) {
        ESP_LOGW(TAG, "Invalid brightness value: %d (must be 0-100)", brightness);
        return;
    }
    
    setBrightness(brightness);
    ESP_LOGI(TAG, "Brightness set to %d%% via MQTT", brightness);
}

void DeviceControlManager::processHapticCommand(const char* message) {
    bool enable = false;
    
    if (strcmp(message, "on") == 0 || strcmp(message, "true") == 0 || strcmp(message, "1") == 0 || strcmp(message, "enable") == 0) {
        enable = true;
    } else if (strcmp(message, "off") == 0 || strcmp(message, "false") == 0 || strcmp(message, "0") == 0 || strcmp(message, "disable") == 0) {
        enable = false;
    } else if (strcmp(message, "toggle") == 0) {
        enable = !haptic_enabled;
    } else {
        ESP_LOGW(TAG, "Invalid haptic command: %s (use: on/off/toggle)", message);
        return;
    }
    
    setHapticEnabled(enable);
    ESP_LOGI(TAG, "Haptic feedback %s via MQTT", enable ? "enabled" : "disabled");
}

void DeviceControlManager::processDeviceCommand(const char* message) {
    ESP_LOGI(TAG, "Processing device command: %s", message);
    
    if (strcmp(message, "reboot") == 0 || strcmp(message, "restart") == 0) {
        ESP_LOGI(TAG, "Reboot command received - restarting in 2 seconds");
        vTaskDelay(pdMS_TO_TICKS(2000));
        esp_restart();
        
    } else if (strcmp(message, "factory_reset") == 0) {
        ESP_LOGI(TAG, "Factory reset command received");
        // TODO: Implement factory reset functionality
        // This should match the settings screen factory reset
        
    } else if (strcmp(message, "wifi_reset") == 0) {
        ESP_LOGI(TAG, "WiFi reset command received");
        // TODO: Implement WiFi credentials reset
        // This should match the settings screen WiFi reset
        
    } else if (strcmp(message, "status") == 0) {
        ESP_LOGI(TAG, "Status request received");
        publishStatus();
        
    } else {
        ESP_LOGW(TAG, "Unknown device command: %s", message);
    }
}

void DeviceControlManager::setBrightness(int brightness) {
    // Clamp to valid range
    if (brightness < 0) brightness = 0;
    if (brightness > 100) brightness = 100;
    
    current_brightness = brightness;
    
    // Convert percentage to actual display brightness value
    // This depends on your display implementation
    // TODO: Call your display brightness function
    // display_set_brightness(brightness);
    
    // Publish updated brightness
    publishBrightness(brightness);
    
    ESP_LOGI(TAG, "Display brightness set to %d%%", brightness);
}

void DeviceControlManager::setHapticEnabled(bool enabled) {
    haptic_enabled = enabled;
    
    // TODO: Configure haptic feedback hardware
    // This depends on your haptic implementation
    // haptic_set_enabled(enabled);
    
    // Publish updated haptic status  
    publishHapticStatus(enabled);
    
    ESP_LOGI(TAG, "Haptic feedback %s", enabled ? "enabled" : "disabled");
}

void DeviceControlManager::publishStatus() {
    if (!MQTTManager::isConnected()) {
        return;
    }
    
    // Create JSON status message
    char status_buffer[512];
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);
    
    uint32_t uptime_seconds = (xTaskGetTickCount() * portTICK_PERIOD_MS) / 1000;
    uint32_t free_heap = esp_get_free_heap_size();
    
    snprintf(status_buffer, sizeof(status_buffer), 
        "{"
        "\"wifi\":true,"
        "\"mqtt\":true,"
        "\"ip\":\"%d.%d.%d.%d\","
        "\"brightness\":%d,"
        "\"haptic\":%s,"
        "\"uptime\":%lu,"
        "\"free_heap\":%lu"
        "}",
        IP2STR(&ip_info.ip),
        current_brightness,
        haptic_enabled ? "true" : "false",
        (unsigned long)uptime_seconds,
        (unsigned long)free_heap);
    
    MQTTManager::publish("home/knob/status", status_buffer, true);
    ESP_LOGI(TAG, "Published device status");
}

void DeviceControlManager::publishBrightness(int brightness) {
    if (MQTTManager::isConnected()) {
        char brightness_str[16];
        snprintf(brightness_str, sizeof(brightness_str), "%d", brightness);
        MQTTManager::publish("home/knob/brightness/status", brightness_str);
    }
}

void DeviceControlManager::publishHapticStatus(bool enabled) {
    if (MQTTManager::isConnected()) {
        MQTTManager::publish("home/knob/haptic/status", enabled ? "on" : "off");
    }
}
