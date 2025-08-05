#include "device_control_manager.h"
#include "../../core/display.h"  // For brightness control
#include "../settings/settings_screen.h"  // For settings integration

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

void DeviceControlManager::handleMQTTMessage(const String& topic, const String& message) {
    ESP_LOGI(TAG, "Processing device control message: %s = %s", topic.c_str(), message.c_str());
    
    if (topic == "home/knob/command") {
        processDeviceCommand(message);
    } else if (topic == "home/knob/brightness") {
        processBrightnessCommand(message);
    } else if (topic == "home/knob/haptic") {
        processHapticCommand(message);
    } else {
        ESP_LOGW(TAG, "Unknown device control topic: %s", topic.c_str());
    }
}

void DeviceControlManager::processBrightnessCommand(const String& message) {
    int brightness = message.toInt();
    
    // Validate range (0-100%)
    if (brightness < 0 || brightness > 100) {
        ESP_LOGW(TAG, "Invalid brightness value: %d (must be 0-100)", brightness);
        return;
    }
    
    setBrightness(brightness);
    ESP_LOGI(TAG, "Brightness set to %d%% via MQTT", brightness);
}

void DeviceControlManager::processHapticCommand(const String& message) {
    bool enable = false;
    
    if (message == "on" || message == "true" || message == "1" || message == "enable") {
        enable = true;
    } else if (message == "off" || message == "false" || message == "0" || message == "disable") {
        enable = false;
    } else if (message == "toggle") {
        enable = !haptic_enabled;
    } else {
        ESP_LOGW(TAG, "Invalid haptic command: %s (use: on/off/toggle)", message.c_str());
        return;
    }
    
    setHapticEnabled(enable);
    ESP_LOGI(TAG, "Haptic feedback %s via MQTT", enable ? "enabled" : "disabled");
}

void DeviceControlManager::processDeviceCommand(const String& message) {
    ESP_LOGI(TAG, "Processing device command: %s", message.c_str());
    
    if (message == "reboot" || message == "restart") {
        ESP_LOGI(TAG, "Reboot command received - restarting in 2 seconds");
        vTaskDelay(pdMS_TO_TICKS(2000));
        esp_restart();
        
    } else if (message == "factory_reset") {
        ESP_LOGI(TAG, "Factory reset command received");
        // TODO: Implement factory reset functionality
        // This should match the settings screen factory reset
        
    } else if (message == "wifi_reset") {
        ESP_LOGI(TAG, "WiFi reset command received");
        // TODO: Implement WiFi credentials reset
        // This should match the settings screen WiFi reset
        
    } else if (message == "status") {
        ESP_LOGI(TAG, "Status request received");
        publishStatus();
        
    } else {
        ESP_LOGW(TAG, "Unknown device command: %s", message.c_str());
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
    String status = "{";
    status += "\"wifi\":true,";
    status += "\"mqtt\":true,";
    status += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    status += "\"brightness\":" + String(current_brightness) + ",";
    status += "\"haptic\":" + String(haptic_enabled ? "true" : "false") + ",";
    status += "\"uptime\":" + String(millis()/1000) + ",";
    status += "\"free_heap\":" + String(ESP.getFreeHeap());
    status += "}";
    
    MQTTManager::publish("home/knob/status", status, true);
    ESP_LOGI(TAG, "Published device status");
}

void DeviceControlManager::publishBrightness(int brightness) {
    if (MQTTManager::isConnected()) {
        MQTTManager::publish("home/knob/brightness/status", String(brightness));
    }
}

void DeviceControlManager::publishHapticStatus(bool enabled) {
    if (MQTTManager::isConnected()) {
        MQTTManager::publish("home/knob/haptic/status", enabled ? "on" : "off");
    }
}
