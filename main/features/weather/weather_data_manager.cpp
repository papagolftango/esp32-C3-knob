#include "weather_data_manager.h"
#include "esp_log.h"
#include "../../core/mqtt_manager.h"
#include <sstream>
#include <iomanip>
#include <cstring>

static const char* TAG = "weather_data";

// Static member definitions
float WeatherDataManager::current_temperature = 0.0f;
float WeatherDataManager::current_humidity = 0.0f;
bool WeatherDataManager::frost_risk_active = false;

bool WeatherDataManager::temperature_changed = false;
bool WeatherDataManager::humidity_changed = false;
bool WeatherDataManager::frost_risk_changed = false;
bool WeatherDataManager::initialized = false;

void WeatherDataManager::begin() {
    if (initialized) return;
    
    current_temperature = getDefaultTemperature();
    current_humidity = getDefaultHumidity();
    frost_risk_active = getDefaultFrostRisk();
    
    temperature_changed = false;
    humidity_changed = false;
    frost_risk_changed = false;
    initialized = true;
    
    // Register MQTT handler for weather data
    MQTTManager::registerHandler("home/weather", handleMQTTMessage);
    
    ESP_LOGI(TAG, "Weather Data Manager initialised");
}

void WeatherDataManager::updateTemperature(float temp_celsius) {
    if (!initialized) {
        begin();
    }
    
    // Only update if the temperature has actually changed (with small tolerance)
    if (abs(temp_celsius - current_temperature) > 0.1f) {
        current_temperature = temp_celsius;
        temperature_changed = true;
        ESP_LOGI(TAG, "Temperature updated: %.1f°C", temp_celsius);
    }
}

float WeatherDataManager::getCurrentTemperature() {
    if (!initialized) {
        begin();
    }
    return current_temperature;
}

bool WeatherDataManager::hasTemperatureChanged() {
    return temperature_changed;
}

void WeatherDataManager::updateHumidity(float humidity_percent) {
    if (!initialized) {
        begin();
    }
    
    // Clamp humidity to valid range
    if (humidity_percent < 0.0f) humidity_percent = 0.0f;
    if (humidity_percent > 100.0f) humidity_percent = 100.0f;
    
    // Only update if humidity has actually changed (with small tolerance)
    if (abs(humidity_percent - current_humidity) > 0.5f) {
        current_humidity = humidity_percent;
        humidity_changed = true;
        ESP_LOGI(TAG, "Humidity updated: %.1f%%", humidity_percent);
    }
}

float WeatherDataManager::getCurrentHumidity() {
    if (!initialized) {
        begin();
    }
    return current_humidity;
}

bool WeatherDataManager::hasHumidityChanged() {
    return humidity_changed;
}

void WeatherDataManager::updateFrostRisk(bool frost_risk) {
    if (!initialized) {
        begin();
    }
    
    // Only update if frost risk has actually changed
    if (frost_risk != frost_risk_active) {
        frost_risk_active = frost_risk;
        frost_risk_changed = true;
        ESP_LOGI(TAG, "Frost risk updated: %s", frost_risk ? "ACTIVE" : "CLEAR");
    }
}

bool WeatherDataManager::getFrostRisk() {
    if (!initialized) {
        begin();
    }
    return frost_risk_active;
}

bool WeatherDataManager::hasFrostRiskChanged() {
    return frost_risk_changed;
}

void WeatherDataManager::clearChangeFlags() {
    temperature_changed = false;
    humidity_changed = false;
    frost_risk_changed = false;
}

bool WeatherDataManager::hasAnyDataChanged() {
    return temperature_changed || humidity_changed || frost_risk_changed;
}

std::string WeatherDataManager::getTemperatureString() {
    if (!initialized) {
        begin();
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << current_temperature << "°C";
    return ss.str();
}

std::string WeatherDataManager::getHumidityString() {
    if (!initialized) {
        begin();
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << current_humidity << "%";
    return ss.str();
}

std::string WeatherDataManager::getFrostRiskString() {
    if (!initialized) {
        begin();
    }
    
    return frost_risk_active ? "FROST RISK" : "NO FROST";
}

float WeatherDataManager::getDefaultTemperature() {
    return 18.5f; // Default to reasonable UK indoor temperature
}

float WeatherDataManager::getDefaultHumidity() {
    return 45.0f; // Default to reasonable UK indoor humidity
}

bool WeatherDataManager::getDefaultFrostRisk() {
    return false; // Default to no frost risk
}

void WeatherDataManager::handleMQTTMessage(const char* topic, const char* message) {
    ESP_LOGI(TAG, "Weather MQTT: %s = %s", topic, message);
    
    if (strcmp(topic, "home/weather/temperature") == 0) {
        float temp = atof(message);
        updateTemperature(temp);
    } 
    else if (strcmp(topic, "home/weather/humidity") == 0) {
        float humidity = atof(message);
        updateHumidity(humidity);
    }
    else if (strcmp(topic, "home/weather/frost_risk") == 0) {
        bool frost_risk = (strcmp(message, "true") == 0 || strcmp(message, "1") == 0 || strcmp(message, "yes") == 0);
        updateFrostRisk(frost_risk);
    }
}
