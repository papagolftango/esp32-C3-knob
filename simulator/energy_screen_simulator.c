// LVGL PC Simulator Version of Energy Screen
// Copy this into LVGL.Simulator/main.c after setting up the PC simulator

#include "lvgl/lvgl.h"
#include <stdio.h>
#include <math.h>

// Mock energy data for simulator
static float energy_balance = -1200.0f;  // Export
static float energy_solar = 2500.0f;     // Solar generation
static float energy_used = 1000.0f;      // House usage
static float energy_vrms = 240.0f;       // Voltage
static float daily_peak_export = -2800.0f;
static float daily_peak_import = 3200.0f;

// Helper function to create colored arc
static void create_colored_arc(lv_obj_t *parent, int16_t x, int16_t y, int16_t radius, 
                               int16_t start_angle, int16_t end_angle, 
                               lv_color_t color, int16_t width) {
    lv_obj_t *arc = lv_arc_create(parent);
    lv_obj_set_size(arc, radius * 2, radius * 2);
    lv_obj_set_pos(arc, x - radius, y - radius);
    lv_arc_set_range(arc, 0, 360);
    lv_arc_set_value(arc, (end_angle - start_angle));
    lv_arc_set_bg_angles(arc, start_angle, end_angle);
    lv_obj_set_style_arc_width(arc, width, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, color, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 0, LV_PART_MAIN);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
}

static void create_energy_screen_simulator(void) {
    // Clear screen
    lv_obj_clean(lv_scr_act());
    
    // Set black background
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
    
    // Title with tariff indicator
    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "⚡ ENERGY 🟢");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Screen indicator
    lv_obj_t *indicator = lv_label_create(lv_scr_act());
    lv_label_set_text(indicator, "1/4 - Rotate to change");
    lv_obj_set_style_text_color(indicator, lv_color_white(), 0);
    lv_obj_set_style_text_font(indicator, &lv_font_montserrat_10, 0);
    lv_obj_align(indicator, LV_ALIGN_TOP_RIGHT, -5, 5);
    
    // Calculate center and scales
    int16_t center_x = 180;  // 360/2
    int16_t center_y = 180;  // 360/2
    int16_t balance_radius = 100;
    int16_t mini_radius = 30;
    
    // Main balance arc (large, center)
    float balance_range = 12000.0f;  // -4000 to +8000W
    float balance_normalized = (energy_balance + 4000.0f) / balance_range;
    int16_t balance_angle = (int16_t)(balance_normalized * 360.0f);
    
    // Determine balance arc color
    lv_color_t balance_color;
    if (energy_balance < -1500) {
        balance_color = lv_color_make(0, 255, 0);  // Green - good export
    } else if (energy_balance < 0) {
        balance_color = lv_color_make(255, 165, 0);  // Orange - low export
    } else if (energy_balance > 0) {
        balance_color = lv_color_make(255, 0, 0);  // Red - import
    } else {
        balance_color = lv_color_make(128, 128, 128);  // Grey - balanced
    }
    
    // Draw main balance arc background
    create_colored_arc(lv_scr_act(), center_x, center_y, balance_radius, 0, 360, 
                       lv_color_make(40, 40, 40), 8);
    
    // Draw main balance arc indicator
    create_colored_arc(lv_scr_act(), center_x, center_y, balance_radius, 
                       270, 270 + balance_angle, balance_color, 8);
    
    // Balance center text
    lv_obj_t *balance_label = lv_label_create(lv_scr_act());
    char balance_text[50];
    if (energy_balance < 0) {
        sprintf(balance_text, "EXPORT\n%.0fW", -energy_balance);
    } else if (energy_balance > 0) {
        sprintf(balance_text, "IMPORT\n%.0fW", energy_balance);
    } else {
        sprintf(balance_text, "BALANCED\n0W");
    }
    lv_label_set_text(balance_label, balance_text);
    lv_obj_set_style_text_color(balance_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(balance_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(balance_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(balance_label);
    
    // Left mini arc - Solar (yellow)
    int16_t solar_x = 60;
    int16_t solar_y = 140;
    float solar_normalized = energy_solar / 5000.0f;
    int16_t solar_angle = (int16_t)(solar_normalized * 180.0f);
    
    create_colored_arc(lv_scr_act(), solar_x, solar_y, mini_radius, 180, 360, 
                       lv_color_make(60, 60, 60), 4);
    create_colored_arc(lv_scr_act(), solar_x, solar_y, mini_radius, 
                       180, 180 + solar_angle, lv_color_make(255, 255, 0), 4);
    
    // Solar label and value
    lv_obj_t *solar_icon = lv_label_create(lv_scr_act());
    lv_label_set_text(solar_icon, "☀️");
    lv_obj_set_pos(solar_icon, solar_x - 15, solar_y - 60);
    
    lv_obj_t *solar_value = lv_label_create(lv_scr_act());
    char solar_text[20];
    if (energy_solar >= 1000) {
        sprintf(solar_text, "%.1fk", energy_solar / 1000.0f);
    } else {
        sprintf(solar_text, "%.0f", energy_solar);
    }
    lv_label_set_text(solar_value, solar_text);
    lv_obj_set_style_text_color(solar_value, lv_color_white(), 0);
    lv_obj_set_style_text_font(solar_value, &lv_font_montserrat_12, 0);
    lv_obj_set_pos(solar_value, solar_x - 15, solar_y - 10);
    
    // Right mini arc - Usage (blue)
    int16_t usage_x = 300;
    int16_t usage_y = 140;
    float usage_normalized = energy_used / 8000.0f;
    int16_t usage_angle = (int16_t)(usage_normalized * 180.0f);
    
    create_colored_arc(lv_scr_act(), usage_x, usage_y, mini_radius, 180, 360, 
                       lv_color_make(60, 60, 60), 4);
    create_colored_arc(lv_scr_act(), usage_x, usage_y, mini_radius, 
                       180, 180 + usage_angle, lv_color_make(0, 150, 255), 4);
    
    // Usage label and value
    lv_obj_t *usage_icon = lv_label_create(lv_scr_act());
    lv_label_set_text(usage_icon, "🏠");
    lv_obj_set_pos(usage_icon, usage_x - 15, usage_y - 60);
    
    lv_obj_t *usage_value = lv_label_create(lv_scr_act());
    char usage_text[20];
    if (energy_used >= 1000) {
        sprintf(usage_text, "%.1fk", energy_used / 1000.0f);
    } else {
        sprintf(usage_text, "%.0f", energy_used);
    }
    lv_label_set_text(usage_value, usage_text);
    lv_obj_set_style_text_color(usage_value, lv_color_white(), 0);
    lv_obj_set_style_text_font(usage_value, &lv_font_montserrat_12, 0);
    lv_obj_set_pos(usage_value, usage_x - 15, usage_y - 10);
    
    // Peak dots on main arc
    // Export peak (green dot)
    if (daily_peak_export < 0) {
        float peak_export_norm = (-daily_peak_export + 4000.0f) / balance_range;
        float peak_export_angle = peak_export_norm * 2.0f * M_PI - M_PI/2;
        int16_t dot_x = center_x + (int16_t)(balance_radius * cos(peak_export_angle));
        int16_t dot_y = center_y + (int16_t)(balance_radius * sin(peak_export_angle));
        
        lv_obj_t *export_dot = lv_obj_create(lv_scr_act());
        lv_obj_set_size(export_dot, 8, 8);
        lv_obj_set_pos(export_dot, dot_x - 4, dot_y - 4);
        lv_obj_set_style_bg_color(export_dot, lv_color_make(0, 255, 0), 0);
        lv_obj_set_style_radius(export_dot, 4, 0);
        lv_obj_set_style_border_width(export_dot, 1, 0);
        lv_obj_set_style_border_color(export_dot, lv_color_white(), 0);
    }
    
    // Import peak (red dot)
    if (daily_peak_import > 0) {
        float peak_import_norm = (daily_peak_import + 4000.0f) / balance_range;
        float peak_import_angle = peak_import_norm * 2.0f * M_PI - M_PI/2;
        int16_t dot_x = center_x + (int16_t)(balance_radius * cos(peak_import_angle));
        int16_t dot_y = center_y + (int16_t)(balance_radius * sin(peak_import_angle));
        
        lv_obj_t *import_dot = lv_obj_create(lv_scr_act());
        lv_obj_set_size(import_dot, 8, 8);
        lv_obj_set_pos(import_dot, dot_x - 4, dot_y - 4);
        lv_obj_set_style_bg_color(import_dot, lv_color_make(255, 0, 0), 0);
        lv_obj_set_style_radius(import_dot, 4, 0);
        lv_obj_set_style_border_width(import_dot, 1, 0);
        lv_obj_set_style_border_color(import_dot, lv_color_white(), 0);
    }
    
    // Bottom status line
    lv_obj_t *status = lv_label_create(lv_scr_act());
    char status_text[50];
    sprintf(status_text, "📡 EmonTX3 | %.0fV", energy_vrms);
    lv_label_set_text(status, status_text);
    lv_obj_set_style_text_color(status, lv_color_make(180, 180, 180), 0);
    lv_obj_set_style_text_font(status, &lv_font_montserrat_12, 0);
    lv_obj_align(status, LV_ALIGN_BOTTOM_MID, 0, -30);
    
    // Instructions
    lv_obj_t *instructions = lv_label_create(lv_scr_act());
    lv_label_set_text(instructions, "Touch to interact");
    lv_obj_set_style_text_color(instructions, lv_color_make(128, 128, 128), 0);
    lv_obj_set_style_text_font(instructions, &lv_font_montserrat_10, 0);
    lv_obj_align(instructions, LV_ALIGN_BOTTOM_MID, 0, -10);
}

// Main function for simulator
int main(void) {
    // Initialize LVGL
    lv_init();
    
    // Initialize display (simulator handles this)
    // ... simulator initialization code ...
    
    // Create the energy screen
    create_energy_screen_simulator();
    
    // Main loop
    while(1) {
        lv_timer_handler();
        // Simulator handles delay
    }
    
    return 0;
}
