#include "energy_ui.h"
#include "../../core/hardware/haptic_feedback.h"
#include <cmath>

void EnergyUI::createScreen() {
    // This will be called to create the energy screen
    // Data will be passed separately for updates
}

void EnergyUI::updateScreen(const EnergyData& data, const PeakData& peaks) {
    // Create title with tariff indicator
    lv_obj_t *title = lv_label_create(lv_scr_act());
    String title_text = "⚡ ENERGY";
    
    // Add tariff indicator based on data
    if (data.tariff == 1 || data.tariff == 4) {
        title_text += " 🟢";  // Green for low tariff (night/off-peak)
    } else {
        title_text += " 🔴";  // Red for high tariff
    }
    
    lv_label_set_text(title, title_text.c_str());
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
    
    createMainBalanceArc(data);
    createPeakDots(peaks);
    
    if (data.solar > 0) {
        createSolarArc(data);
    }
    
    if (data.used > 0) {
        createUsageArc(data);
    }
    
    createStatusDisplay(data, data.valid);
}

void EnergyUI::createMainBalanceArc(const EnergyData& data) {
    const float max_scale_export = 4000.0;  // 4kW export max
    const float max_scale_import = 8000.0;  // 8kW import max
    
    int arc_value = 0;
    lv_color_t arc_color = getBalanceColor(data.balance, data.solar);
    
    if (data.balance < 0) {
        // Exporting (excess solar)
        arc_value = calculateArcValue(abs(data.balance), max_scale_export);
    } else if (data.balance > 0) {
        // Importing
        arc_value = calculateArcValue(data.balance, max_scale_import);
    }
    
    // Create main balance arc
    lv_obj_t *balance_arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(balance_arc, 200, 200);
    lv_obj_center(balance_arc);
    lv_arc_set_rotation(balance_arc, 270);
    lv_arc_set_bg_angles(balance_arc, 0, 360);
    lv_arc_set_value(balance_arc, arc_value);
    lv_obj_remove_style(balance_arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_color(balance_arc, arc_color, LV_PART_INDICATOR);
    
    // Main balance display (center of main arc)
    lv_obj_t *balance_label = lv_label_create(lv_scr_act());
    String balance_text = "";
    
    if (data.balance < 0) {
        balance_text = "EXPORT\\n" + String(abs(data.balance), 0) + "W";
    } else if (data.balance > 0) {
        balance_text = "IMPORT\\n" + String(data.balance, 0) + "W";
    } else {
        balance_text = "BALANCED\\n0W";
    }
    
    lv_label_set_text(balance_label, balance_text.c_str());
    lv_obj_set_style_text_font(balance_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(balance_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(balance_label);
}

void EnergyUI::createPeakDots(const PeakData& peaks) {
    const float max_scale_export = 4000.0;
    const float max_scale_import = 8000.0;
    
    // Export peak dot (green)
    if (peaks.daily_export_peak < 0) {
        lv_obj_t *export_dot = lv_obj_create(lv_scr_act());
        lv_obj_set_size(export_dot, 8, 8);
        lv_obj_set_style_bg_color(export_dot, lv_palette_main(LV_PALETTE_GREEN), 0);
        lv_obj_set_style_border_width(export_dot, 2, 0);
        lv_obj_set_style_border_color(export_dot, lv_color_white(), 0);
        lv_obj_set_style_radius(export_dot, LV_RADIUS_CIRCLE, 0);
        
        positionPeakDot(export_dot, abs(peaks.daily_export_peak), max_scale_export);
    }
    
    // Import peak dot (red)
    if (peaks.daily_import_peak > 0) {
        lv_obj_t *import_dot = lv_obj_create(lv_scr_act());
        lv_obj_set_size(import_dot, 8, 8);
        lv_obj_set_style_bg_color(import_dot, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_set_style_border_width(import_dot, 2, 0);
        lv_obj_set_style_border_color(import_dot, lv_color_white(), 0);
        lv_obj_set_style_radius(import_dot, LV_RADIUS_CIRCLE, 0);
        
        positionPeakDot(import_dot, peaks.daily_import_peak, max_scale_import);
    }
}

void EnergyUI::createSolarArc(const EnergyData& data) {
    lv_obj_t *solar_arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(solar_arc, 60, 60);
    lv_obj_align(solar_arc, LV_ALIGN_LEFT_MID, 20, 0);
    lv_arc_set_rotation(solar_arc, 270);
    lv_arc_set_bg_angles(solar_arc, 0, 180);  // Half circle
    
    int solar_value = calculateArcValue(data.solar, 5000.0);  // 0-5000W scale
    lv_arc_set_value(solar_arc, solar_value);
    lv_obj_remove_style(solar_arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_color(solar_arc, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_INDICATOR);
    
    // Solar label
    lv_obj_t *solar_label = lv_label_create(lv_scr_act());
    String solar_text = "☀️\\n" + String(data.solar, 0);
    lv_label_set_text(solar_label, solar_text.c_str());
    lv_obj_set_style_text_font(solar_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_align(solar_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(solar_label, solar_arc, LV_ALIGN_CENTER, 0, 0);
}

void EnergyUI::createUsageArc(const EnergyData& data) {
    lv_obj_t *usage_arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(usage_arc, 60, 60);
    lv_obj_align(usage_arc, LV_ALIGN_RIGHT_MID, -20, 0);
    lv_arc_set_rotation(usage_arc, 270);
    lv_arc_set_bg_angles(usage_arc, 0, 180);  // Half circle
    
    int usage_value = calculateArcValue(data.used, 8000.0);  // 0-8000W scale
    lv_arc_set_value(usage_arc, usage_value);
    lv_obj_remove_style(usage_arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_color(usage_arc, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    
    // Usage label
    lv_obj_t *usage_label = lv_label_create(lv_scr_act());
    String usage_text = "🏠\\n" + String(data.used, 0);
    lv_label_set_text(usage_label, usage_text.c_str());
    lv_obj_set_style_text_font(usage_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_align(usage_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(usage_label, usage_arc, LV_ALIGN_CENTER, 0, 0);
}

void EnergyUI::createStatusDisplay(const EnergyData& data, bool mqtt_connected) {
    lv_obj_t *status = lv_label_create(lv_scr_act());
    String status_text = "";
    
    if (mqtt_connected) {
        status_text = "📡 EmonTX3";
        if (data.vrms > 0) {
            status_text += " | " + String(data.vrms, 0) + "V";
        }
    } else {
        status_text = "📡 Offline";
    }
    
    lv_label_set_text(status, status_text.c_str());
    lv_obj_set_style_text_font(status, &lv_font_montserrat_12, 0);
    lv_obj_align(status, LV_ALIGN_BOTTOM_MID, 0, -10);
}

// Helper functions
int EnergyUI::calculateArcValue(float value, float max_scale) {
    int arc_value = (int)((value / max_scale) * 100);
    return (arc_value > 100) ? 100 : arc_value;
}

lv_color_t EnergyUI::getBalanceColor(float balance, float solar) {
    if (balance < 0) {
        // Exporting - color based on solar generation
        if (solar > 1500) {
            return lv_palette_main(LV_PALETTE_GREEN);  // Good solar
        } else {
            return lv_palette_main(LV_PALETTE_ORANGE); // Low solar
        }
    } else if (balance > 0) {
        return lv_palette_main(LV_PALETTE_RED);        // Importing
    } else {
        return lv_palette_main(LV_PALETTE_GREY);       // Balanced
    }
}

void EnergyUI::positionPeakDot(lv_obj_t* dot, float value, float max_scale) {
    float angle = (value / max_scale) * 360;
    if (angle > 360) angle = 360;
    
    // Position dot on arc circumference
    float rad = (angle - 90) * M_PI / 180.0;  // Convert to radians, offset by 90°
    int dot_x = 180 + (int)(90 * cos(rad));   // 90 = arc radius - dot radius
    int dot_y = 180 + (int)(90 * sin(rad));
    lv_obj_set_pos(dot, dot_x - 4, dot_y - 4);  // Center the dot
}
