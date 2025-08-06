#include "energy_data_manager.h"
#include "../../core/mqtt_manager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG = "ENERGY_DATA";

// Static member definitions
float EnergyDataManager::balance = 0.0f;
float EnergyDataManager::solar = 0.0f;
float EnergyDataManager::imported = 0.0f;
float EnergyDataManager::used = 0.0f;
std::string EnergyDataManager::tariff = "unknown";

// Daily peak tracking
float EnergyDataManager::solar_peak = 0.0f;
float EnergyDataManager::imported_peak = 0.0f;

// Daily accumulation tracking (kWh)
float EnergyDataManager::solar_accumulated = 0.0f;
float EnergyDataManager::imported_accumulated = 0.0f;

// Time tracking for daily reset
unsigned long EnergyDataManager::last_accumulation_time = 0;
int EnergyDataManager::last_reset_day = -1;

bool EnergyDataManager::data_changed = false;
unsigned long EnergyDataManager::last_update = 0;

void EnergyDataManager::begin() {
    ESP_LOGI(TAG, "Initializing Energy Data Manager");
    
    // Initialize daily tracking
    last_accumulation_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    last_reset_day = getCurrentDayOfYear();
    
    ESP_LOGI(TAG, "Daily tracking initialized - Day: %d", last_reset_day);
    
    // Register our handler with the MQTT manager for emon/emontx3 namespace
    MQTTManager::registerHandler("emon/emontx3", handleMQTTMessage);
    
    ESP_LOGI(TAG, "Energy Data Manager initialized - registered for emon/emontx3/* topics");
}

void EnergyDataManager::handleMQTTMessage(const std::string& topic, const std::string& message) {
    ESP_LOGI(TAG, "Processing energy message: %s = %s", topic.c_str(), message.c_str());
    
    float value = std::stof(message);
    bool updated = false;
    
    if (topic == "emon/emontx3/balance") {
        if (isValidPower(value)) {
            balance = value;
            updated = true;
            ESP_LOGI(TAG, "Grid Balance: %.2f W %s", balance, 
                     balance > 0 ? "(importing)" : balance < 0 ? "(exporting)" : "(balanced)");
        } else {
            ESP_LOGW(TAG, "Invalid balance value: %.2f", value);
        }
        
    } else if (topic == "emon/emontx3/solar") {
        if (isValidPower(value)) {
            solar = value;
            updated = true;
            ESP_LOGI(TAG, "Solar Generation: %.2f W", solar);
        } else {
            ESP_LOGW(TAG, "Invalid solar value: %.2f", value);
        }
        
    } else if (topic == "emon/emontx3/import") {
        if (isValidPower(value)) {
            imported = value;
            updated = true;
            ESP_LOGI(TAG, "Energy Imported: %.2f W", imported);
        } else {
            ESP_LOGW(TAG, "Invalid import value: %.2f", value);
        }
        
    } else if (topic == "emon/emontx3/used") {
        if (isValidPower(value)) {
            used = value;
            updated = true;
            ESP_LOGI(TAG, "Energy Used: %.2f W", used);
        } else {
            ESP_LOGW(TAG, "Invalid usage value: %.2f", value);
        }
        
    } else if (topic == "emon/emontx3/tariff") {
        if (message == "high" || message == "low" || message == "peak" || message == "off-peak") {
            tariff = message;
            updated = true;
            ESP_LOGI(TAG, "Tariff Status: %s", tariff.c_str());
        } else {
            ESP_LOGW(TAG, "Invalid tariff value: %s", message.c_str());
        }
        
    } else {
        ESP_LOGW(TAG, "Unknown energy topic: %s", topic.c_str());
    }
    
    if (updated) {
        data_changed = true;
        last_update = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        // Check for daily reset first
        checkForDailyReset();
        
        // Update peak tracking and accumulation
        updatePeaks();
        updateAccumulation();
        
        // Log comprehensive energy status periodically
        static unsigned long last_status_log = 0;
        if ((xTaskGetTickCount() * portTICK_PERIOD_MS) - last_status_log > 30000) { // Every 30 seconds
            ESP_LOGI(TAG, "=== Energy Status ===");
            ESP_LOGI(TAG, "Current: Solar: %.2f W, Imported: %.2f W, Used: %.2f W", solar, imported, used);
            ESP_LOGI(TAG, "Daily Peaks: Solar: %.2f W, Imported: %.2f W", solar_peak, imported_peak);
            ESP_LOGI(TAG, "Daily Total: Solar: %.3f kWh, Imported: %.3f kWh", solar_accumulated, imported_accumulated);
            ESP_LOGI(TAG, "Balance: %.2f W, Tariff: %s", balance, tariff.c_str());
            ESP_LOGI(TAG, "============================");
            last_status_log = xTaskGetTickCount() * portTICK_PERIOD_MS;
        }
    }
}

bool EnergyDataManager::hasDataChanged() {
    if (data_changed) {
        data_changed = false;
        return true;
    }
    return false;
}

bool EnergyDataManager::isDataValid() {
    // Consider data valid if we've received updates recently (within 5 minutes)
    const unsigned long TIMEOUT_MS = 5 * 60 * 1000; // 5 minutes
    return (last_update > 0) && ((xTaskGetTickCount() * portTICK_PERIOD_MS) - last_update < TIMEOUT_MS);
}

bool EnergyDataManager::isValidPower(float value) {
    // Reasonable power range: -20kW to +20kW
    return (value >= -20000.0f && value <= 20000.0f);
}

void EnergyDataManager::updatePeaks() {
    // Update solar peak
    if (solar > solar_peak) {
        solar_peak = solar;
        ESP_LOGI(TAG, "New solar peak: %.2f W", solar_peak);
    }
    
    // Update imported peak
    if (imported > imported_peak) {
        imported_peak = imported;
        ESP_LOGI(TAG, "New imported peak: %.2f W", imported_peak);
    }
}

void EnergyDataManager::updateAccumulation() {
    unsigned long current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    if (last_accumulation_time == 0) {
        last_accumulation_time = current_time;
        return;
    }
    
    // Calculate time delta in hours
    float time_delta_hours = (current_time - last_accumulation_time) / (1000.0f * 3600.0f);
    
    // Only accumulate positive values and reasonable time deltas (< 1 hour to avoid startup issues)
    if (time_delta_hours > 0 && time_delta_hours < 1.0f) {
        // Accumulate solar generation (only positive values)
        if (solar > 0) {
            solar_accumulated += (solar * time_delta_hours) / 1000.0f; // Convert W to kWh
        }
        
        // Accumulate imported energy (only positive values)
        if (imported > 0) {
            imported_accumulated += (imported * time_delta_hours) / 1000.0f; // Convert W to kWh
        }
    }
    
    last_accumulation_time = current_time;
}

void EnergyDataManager::checkForDailyReset() {
    int current_day = getCurrentDayOfYear();
    
    if (last_reset_day != current_day) {
        ESP_LOGI(TAG, "Daily reset triggered - Day changed from %d to %d", last_reset_day, current_day);
        resetDailyValues();
        last_reset_day = current_day;
    }
}

int EnergyDataManager::getCurrentDayOfYear() {
    // Simple day calculation based on ticks since boot
    // In a real implementation, you'd want to use RTC or NTP time
    unsigned long current_time_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
    unsigned long current_time_seconds = current_time_ms / 1000;
    
    // For testing: assume each day is 24 hours = 86400 seconds
    // In production, you'd integrate with ESP32 RTC or NTP
    return (current_time_seconds / 86400) % 365; // Simple day of year calculation
}

void EnergyDataManager::resetDailyValues() {
    ESP_LOGI(TAG, "Resetting daily values - Previous peaks: Solar=%.2f W, Import=%.2f W", 
             solar_peak, imported_peak);
    ESP_LOGI(TAG, "Previous totals: Solar=%.3f kWh, Import=%.3f kWh", 
             solar_accumulated, imported_accumulated);
    
    // Reset peak values
    solar_peak = solar;           // Set to current value, not zero
    imported_peak = imported;     // Set to current value, not zero
    
    // Reset accumulated values
    solar_accumulated = 0.0f;
    imported_accumulated = 0.0f;
    
    // Reset accumulation timing
    last_accumulation_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    ESP_LOGI(TAG, "Daily values reset complete");
}
