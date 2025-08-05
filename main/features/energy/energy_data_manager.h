#pragma once

#include <esp_log.h>
#include <string>

/**
 * Energy Data Manager - Handles EmonTX3 energy monitoring data
 * 
 * This manager demonstrates the app-specific handler pattern for
 * the centralized MQTT architecture. It processes all energy-related
 * MQTT messages from the emon/emontx3/* namespace.
 */

class EnergyDataManager {
public:
    // Initialize the energy data manager and register MQTT handler
    static void begin();
    
    // MQTT message handler for energy topics
    static void handleMQTTMessage(const String& topic, const String& message);
    
    // Get current energy data
    static float getBalance() { return balance; }
    static float getSolar() { return solar; }
    static float getVrms() { return vrms; }
    static float getUsed() { return used; }
    static String getTariff() { return tariff; }
    
    // Check if data has been updated (for UI refresh)
    static bool hasDataChanged();
    
    // Data validation
    static bool isDataValid();
    static unsigned long getLastUpdateTime() { return last_update; }

private:
    static float balance;           // Grid import/export balance (W)
    static float solar;             // Solar generation (W)
    static float vrms;              // Voltage RMS (V)
    static float used;              // Energy consumption (W)
    static String tariff;           // Current tariff status
    
    static bool data_changed;       // Flag for UI updates
    static unsigned long last_update; // Last data update timestamp
    
    // Data validation helpers
    static bool isValidPower(float value);
    static bool isValidVoltage(float value);
};
