#pragma once

#include <string>

class WeatherDataManager {
public:
    static void init();
    
    // Temperature methods
    static void updateTemperature(float temp_celsius);
    static float getCurrentTemperature();
    static bool hasTemperatureChanged();
    
    // Humidity methods  
    static void updateHumidity(float humidity_percent);
    static float getCurrentHumidity();
    static bool hasHumidityChanged();
    
    // Frost risk methods
    static void updateFrostRisk(bool frost_risk);
    static bool getFrostRisk();
    static bool hasFrostRiskChanged();
    
    // Change flag management
    static void clearChangeFlags();
    static bool hasAnyDataChanged();
    
    // Get formatted display strings
    static std::string getTemperatureString();
    static std::string getHumidityString();
    static std::string getFrostRiskString();
    
    // MQTT message handler
    static void handleMQTTMessage(const char* topic, const char* message);

private:
    static float current_temperature;
    static float current_humidity;
    static bool frost_risk_active;
    
    static bool temperature_changed;
    static bool humidity_changed;
    static bool frost_risk_changed;
    static bool initialized;
    
    static float getDefaultTemperature();
    static float getDefaultHumidity();
    static bool getDefaultFrostRisk();
};
