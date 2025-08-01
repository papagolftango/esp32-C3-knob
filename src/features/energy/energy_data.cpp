#include "energy_data.h"
#include "../../core/hardware/haptic_feedback.h"
#include <cmath>

EnergyData EnergyData_Manager::current_data;
PeakData EnergyData_Manager::peak_data;
String EnergyData_Manager::energy_tariff = "";
bool EnergyData_Manager::mock_data_enabled = false;
unsigned long EnergyData_Manager::mock_start_time = 0;
float EnergyData_Manager::mock_time_scale = 0.0f;

void EnergyData_Manager::begin() {
    // Initialize data structures
    current_data = EnergyData();
    peak_data = PeakData();
    
    if (mock_data_enabled) {
        mock_start_time = millis();
        mock_time_scale = 24.0 * 60.0 / 2.0; // 24 hours in 2 minutes
        Serial.println("Energy data manager initialized with mock data");
    } else {
        Serial.println("Energy data manager initialized for real data");
    }
}

void EnergyData_Manager::update() {
    if (mock_data_enabled) {
        generateMockData();
    }
    
    // Check for daily peak reset (noon reset)
    static unsigned long last_check = 0;
    unsigned long now = millis();
    if (now - last_check > 60000) { // Check every minute
        time_t current_time = time(nullptr);
        struct tm* time_info = localtime(&current_time);
        
        if (time_info->tm_hour == 12 && time_info->tm_min == 0) {
            static bool reset_today = false;
            if (!reset_today) {
                resetDailyPeaks();
                reset_today = true;
            }
        } else {
            static bool reset_today = false;
            reset_today = false;
        }
        last_check = now;
    }
}

const EnergyData& EnergyData_Manager::getCurrentData() {
    return current_data;
}

const PeakData& EnergyData_Manager::getPeakData() {
    return peak_data;
}

void EnergyData_Manager::updateBalance(float balance) {
    current_data.balance = balance;
    current_data.valid = true;
    updateDailyPeaks(balance);
}

void EnergyData_Manager::updateSolar(float solar) {
    current_data.solar = solar;
    current_data.valid = true;
}

void EnergyData_Manager::updateUsed(float used) {
    current_data.used = used;
    current_data.valid = true;
}

void EnergyData_Manager::updateVrms(float vrms) {
    current_data.vrms = vrms;
    current_data.valid = true;
}

void EnergyData_Manager::updateTariff(const String& tariff) {
    energy_tariff = tariff;
    
    // Convert tariff string to number
    String tariff_lower(tariff);  // Create a copy for modification
    tariff_lower.toLowerCase();
    
    if (tariff_lower.indexOf("low") >= 0 || 
        tariff_lower.indexOf("off") >= 0 ||
        tariff_lower.indexOf("night") >= 0) {
        current_data.tariff = 1;  // Low tariff
    } else {
        current_data.tariff = 2;  // High tariff
    }
    current_data.valid = true;
}

void EnergyData_Manager::updateDailyPeaks(float balance) {
    bool peak_reached = isPeakReached(balance);
    
    if (balance < 0 && balance < peak_data.daily_export_peak) {
        peak_data.daily_export_peak = balance;
        peak_data.last_peak_update = millis();
        if (peak_reached && !peak_data.export_peak_reached_today) {
            peak_data.export_peak_reached_today = true;
            HapticFeedback::peakReached();
            Serial.printf("New export peak reached: %.0fW\\n", abs(balance));
        }
    }
    
    if (balance > 0 && balance > peak_data.daily_import_peak) {
        peak_data.daily_import_peak = balance;
        peak_data.last_peak_update = millis();
        if (peak_reached && !peak_data.import_peak_reached_today) {
            peak_data.import_peak_reached_today = true;
            HapticFeedback::peakReached();
            Serial.printf("New import peak reached: %.0fW\\n", balance);
        }
    }
}

void EnergyData_Manager::resetDailyPeaks() {
    peak_data.daily_export_peak = 0.0f;
    peak_data.daily_import_peak = 0.0f;
    peak_data.export_peak_reached_today = false;
    peak_data.import_peak_reached_today = false;
    peak_data.last_peak_update = millis();
    Serial.println("Daily energy peaks reset at noon");
}

void EnergyData_Manager::enableMockData(bool enable) {
    mock_data_enabled = enable;
    if (enable) {
        mock_start_time = millis();
        Serial.println("Mock data enabled");
    } else {
        Serial.println("Mock data disabled");
    }
}

void EnergyData_Manager::generateMockData() {
    unsigned long elapsed = millis() - mock_start_time;
    float sim_hours = (elapsed / 1000.0) * (mock_time_scale / 60.0);
    float time_of_day = fmod(sim_hours, 24.0);
    
    // Solar generation (sunrise ~6am, peak ~noon, sunset ~6pm)
    if (time_of_day >= 6.0 && time_of_day <= 18.0) {
        float solar_factor = sin((time_of_day - 6.0) * M_PI / 12.0);
        current_data.solar = solar_factor * 4500.0;  // Up to 4.5kW peak
    } else {
        current_data.solar = 0.0;
    }
    
    // House usage (varies throughout day)
    float base_usage = 800.0;  // Base load
    float daily_variation = 200.0 * sin((time_of_day - 6.0) * M_PI / 12.0);
    if (time_of_day >= 17.0 && time_of_day <= 22.0) {
        daily_variation += 1000.0;  // Evening peak
    }
    current_data.used = base_usage + daily_variation + (random(-100, 100));
    
    // Balance calculation
    current_data.balance = current_data.used - current_data.solar;
    
    // Mock voltage
    current_data.vrms = 240.0 + random(-5, 5);
    
    // Mock tariff (simple day/night)
    current_data.tariff = (time_of_day >= 22.0 || time_of_day <= 6.0) ? 1 : 2;
    
    current_data.valid = true;
    
    // Update peaks
    updateDailyPeaks(current_data.balance);
}

bool EnergyData_Manager::isPeakReached(float current_balance) {
    // Simple threshold - could be made more sophisticated
    if (current_balance < 0) {
        return abs(current_balance) > abs(peak_data.daily_export_peak) + 100;  // 100W threshold
    } else {
        return current_balance > peak_data.daily_import_peak + 100;  // 100W threshold
    }
}
