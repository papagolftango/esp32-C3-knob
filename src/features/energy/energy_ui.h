#pragma once
#include "energy_data.h"
#include <lvgl.h>

class EnergyUI {
public:
    static void createScreen();
    static void updateScreen(const EnergyData& data, const PeakData& peaks);

private:
    static void createMainBalanceArc(const EnergyData& data);
    static void createPeakDots(const PeakData& peaks);
    static void createSolarArc(const EnergyData& data);
    static void createUsageArc(const EnergyData& data);
    static void createStatusDisplay(const EnergyData& data, bool mqtt_connected);
    
    // Arc calculation helpers
    static int calculateArcValue(float value, float max_scale);
    static lv_color_t getBalanceColor(float balance, float solar);
    static void positionPeakDot(lv_obj_t* dot, float value, float max_scale);
};
