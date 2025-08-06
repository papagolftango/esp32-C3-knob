#include "energy_data_manager.h"
#include "../../core/mqtt_manager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG = "ENERGY_DATA";

// Static member definitions
float EnergyDataManager::balance = 0.0f;
float EnergyDataManager::solar = 0.0f;
float EnergyDataManager::vrms = 0.0f;
float EnergyDataManager::used = 0.0f;
std::string EnergyDataManager::tariff = "unknown";

bool EnergyDataManager::data_changed = false;
unsigned long EnergyDataManager::last_update = 0;

void EnergyDataManager::begin() {
    ESP_LOGI(TAG, "Initializing Energy Data Manager");
    
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
        
    } else if (topic == "emon/emontx3/vrms") {
        if (isValidVoltage(value)) {
            vrms = value;
            updated = true;
            ESP_LOGI(TAG, "Voltage RMS: %.2f V", vrms);
        } else {
            ESP_LOGW(TAG, "Invalid voltage value: %.2f", value);
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
        
        // Log comprehensive energy status periodically
        static unsigned long last_status_log = 0;
        if ((xTaskGetTickCount() * portTICK_PERIOD_MS) - last_status_log > 30000) { // Every 30 seconds
            ESP_LOGI(TAG, "=== Energy Status ===");
            ESP_LOGI(TAG, "Balance: %.2f W, Solar: %.2f W, Used: %.2f W", balance, solar, used);
            ESP_LOGI(TAG, "Voltage: %.2f V, Tariff: %s", vrms, tariff.c_str());
            ESP_LOGI(TAG, "==================");
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

bool EnergyDataManager::isValidVoltage(float value) {
    // Reasonable voltage range: 200V to 260V for UK mains
    return (value >= 200.0f && value <= 260.0f);
}
