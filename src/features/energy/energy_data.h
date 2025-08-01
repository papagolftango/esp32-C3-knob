#pragma once
#include "../../ui_common/data_types.h"
#include <Arduino.h>

class EnergyData_Manager {
public:
    static void begin();
    static void update();
    static const EnergyData& getCurrentData();
    static const PeakData& getPeakData();
    
    // MQTT data updates
    static void updateBalance(float balance);
    static void updateSolar(float solar);
    static void updateUsed(float used);
    static void updateVrms(float vrms);
    static void updateTariff(const String& tariff);
    
    // Peak tracking
    static void updateDailyPeaks(float balance);
    static void resetDailyPeaks();
    
    // Mock data for testing
    static void enableMockData(bool enable);
    static void generateMockData();

private:
    static EnergyData current_data;
    static PeakData peak_data;
    static String energy_tariff;
    static bool mock_data_enabled;
    
    // Mock data simulation
    static unsigned long mock_start_time;
    static float mock_time_scale;
    
    static bool isPeakReached(float current_balance);
};
