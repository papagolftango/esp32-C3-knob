#pragma once

#include <esp_log.h>
#include <string>

/**
 * Energy Data Manager - Handles EmonTX3 energy monitoring data
 * 
 * This manager demonstrates the app-specific handler pattern for
 * the centralized MQTT architecture. It processes all energy-related
 * MQTT messages from the emon/emontx3/ namespace.
 */

class EnergyDataManager {
public:
    // Initialize the energy data manager and register MQTT handler
    static void begin();
    
    // MQTT message handler for energy topics
    static void handleMQTTMessage(const std::string& topic, const std::string& message);
    
    // Get current energy data
    static float getBalance() { return balance; }
    static float getSolar() { return solar; }
    static float getImported() { return imported; }
    static float getUsed() { return used; }
    static std::string getTariff() { return tariff; }
    
    // Get daily peak values
    static float getSolarPeak() { return solar_peak; }
    static float getImportedPeak() { return imported_peak; }
    
    // Get daily accumulated values (kWh)
    static float getSolarAccumulated() { return solar_accumulated; }
    static float getImportedAccumulated() { return imported_accumulated; }
    
    // Manual reset function (for testing or manual reset)
    static void resetDailyValues();
    
    // Check if data has been updated (for UI refresh)
    static bool hasDataChanged();
    
    // Data validation
    static bool isDataValid();
    static unsigned long getLastUpdateTime() { return last_update; }

private:
    static float balance;           // Grid import/export balance (W)
    static float solar;             // Solar generation (W)
    static float imported;          // Energy imported from grid (W)
    static float used;              // Energy consumption (W)
    static std::string tariff;      // Current tariff status
    
    // Daily peak tracking
    static float solar_peak;        // Daily peak solar generation (W)
    static float imported_peak;     // Daily peak imported power (W)
    
    // Daily accumulation tracking (kWh)
    static float solar_accumulated;    // Daily accumulated solar generation (kWh)
    static float imported_accumulated; // Daily accumulated imported energy (kWh)
    
    // Time tracking for daily reset
    static unsigned long last_accumulation_time; // Last time accumulation was calculated
    static int last_reset_day;      // Day of year when last reset occurred
    
    static bool data_changed;       // Flag for UI updates
    static unsigned long last_update; // Last data update timestamp
    
    // Data validation helpers
    static bool isValidPower(float value);
    
    // Internal daily tracking functions
    static void updatePeaks();
    static void updateAccumulation();
    static void checkForDailyReset();
    static int getCurrentDayOfYear();
};
