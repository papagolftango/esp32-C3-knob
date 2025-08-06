#pragma once

#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * Factory Reset Manager
 * 
 * Provides comprehensive factory reset functionality including:
 * - Safe factory reset from settings menu
 * - Emergency recovery mode
 * - Complete NVS flash clearing
 * - WiFi credential removal
 * - Safe restart procedures
 */
class FactoryResetManager {
public:
    /**
     * Initialize the factory reset system
     */
    static void begin();
    
    /**
     * Trigger factory reset from settings menu
     * Safe method for user-initiated reset
     */
    static void triggerFromSettings();
    
    /**
     * Emergency factory reset (immediate)
     * Use only for recovery situations
     */
    static void emergencyReset();
    
    /**
     * Check if device is in factory reset mode
     * Returns true during reset process
     */
    static bool isResetInProgress();

private:
    static bool reset_in_progress;
    
    // Internal reset procedures
    static void performFactoryReset();
    static void clearAllNVSData();
    static void clearWiFiCredentials();
    static void showResetProgress();
    static void safeRestart();
};
