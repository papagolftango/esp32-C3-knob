#include "energy_screen.h"
#include "esp_log.h"

static const char* TAG = "energy_screen";
static lv_obj_t* energy_screen = NULL;
static lv_obj_t* power_label = NULL;
static lv_obj_t* cost_label = NULL;
static lv_obj_t* usage_arc = NULL;

void energy_screen_init(void) {
    // Create screen
    energy_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(energy_screen, lv_color_hex(0x001122), 0);
    
    // Title
    lv_obj_t* title = lv_label_create(energy_screen);
    lv_label_set_text(title, "⚡ ENERGY");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFD700), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Current power usage
    power_label = lv_label_create(energy_screen);
    lv_label_set_text(power_label, "2.34 kW");
    lv_obj_set_style_text_font(power_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(power_label, lv_color_hex(0x00FF00), 0);
    lv_obj_align(power_label, LV_ALIGN_CENTER, 0, -20);
    
    // Daily cost
    cost_label = lv_label_create(energy_screen);
    lv_label_set_text(cost_label, "Today: $12.45");
    lv_obj_set_style_text_color(cost_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(cost_label, LV_ALIGN_CENTER, 0, 10);
    
    // Usage arc (visual indicator)
    usage_arc = lv_arc_create(energy_screen);
    lv_obj_set_size(usage_arc, 80, 80);
    lv_arc_set_rotation(usage_arc, 270);
    lv_arc_set_bg_angles(usage_arc, 0, 360);
    lv_arc_set_value(usage_arc, 65);  // 65% usage
    lv_obj_set_style_arc_color(usage_arc, lv_color_hex(0xFFD700), LV_PART_INDICATOR);
    lv_obj_align(usage_arc, LV_ALIGN_BOTTOM_MID, 0, -20);
    
    // Remove knob (make it display only)
    lv_obj_remove_style(usage_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(usage_arc, LV_OBJ_FLAG_CLICKABLE);
    
    ESP_LOGI(TAG, "Energy screen initialized");
}

void energy_screen_show(void) {
    if (energy_screen) {
        lv_scr_load(energy_screen);
        ESP_LOGI(TAG, "Energy screen shown");
        
        // Update with mock data
        lv_label_set_text(power_label, "2.34 kW");
        lv_label_set_text(cost_label, "Today: $12.45");
        lv_arc_set_value(usage_arc, 65);
    }
}

void energy_screen_handle_button_press(void) {
    ESP_LOGI(TAG, "Energy screen button pressed - refreshing data");
    
    // Mock data update simulation
    static int counter = 0;
    counter++;
    
    switch (counter % 3) {
        case 0:
            lv_label_set_text(power_label, "1.89 kW");
            lv_label_set_text(cost_label, "Today: $11.20");
            lv_arc_set_value(usage_arc, 45);
            break;
        case 1:
            lv_label_set_text(power_label, "3.12 kW");
            lv_label_set_text(cost_label, "Today: $15.67");
            lv_arc_set_value(usage_arc, 85);
            break;
        case 2:
            lv_label_set_text(power_label, "2.34 kW");
            lv_label_set_text(cost_label, "Today: $12.45");
            lv_arc_set_value(usage_arc, 65);
            break;
    }
}
