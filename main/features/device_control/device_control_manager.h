#pragma once

#include <esp_log.h>
#include "../../core/mqtt_manager.h"

/**
 * Device Control Manager - Handles home/knob/ MQTT topics
 * 
 * This manager processes device control commands received via MQTT
 * and integrates with the settings screen functionality.
 */

class DeviceControlManager {
public:
    // Initialize the device control manager and register MQTT handler
    static void begin();
    
    // MQTT message handler for device control topics
    static void handleMQTTMessage(const char* topic, const char* message);
    
    // Publish device status to MQTT
    static void publishStatus();
    static void publishBrightness(int brightness);
    static void publishHapticStatus(bool enabled);
    
    // Settings integration
    static void setBrightness(int brightness);    // 0-100%
    static void setHapticEnabled(bool enabled);   // true/false
    static void processCommand(const char* command);
    
    // Current settings
    static int getCurrentBrightness() { return current_brightness; }
    static bool isHapticEnabled() { return haptic_enabled; }

private:
    static int current_brightness;   // Current screen brightness (0-100)
    static bool haptic_enabled;     // Haptic feedback enabled/disabled
    
    // Command processing
    static void processBrightnessCommand(const char* message);
    static void processHapticCommand(const char* message);
    static void processDeviceCommand(const char* message);
};
