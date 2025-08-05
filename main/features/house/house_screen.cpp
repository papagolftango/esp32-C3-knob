#include "house_screen.h"
#include "esp_log.h"

static const char* TAG = "house_screen";
static lv_obj_t* house_screen = NULL;
static lv_obj_t* lights_label = NULL;
static lv_obj_t* thermostat_label = NULL;
static lv_obj_t* security_label = NULL;
static lv_obj_t* status_label = NULL;

void house_screen_init(void) {
    // Create screen
    house_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(house_screen, lv_color_hex(0x2F1B14), 0);
    
    // Title
    lv_obj_t* title = lv_label_create(house_screen);
    lv_label_set_text(title, "🏠 SMART HOME");
    lv_obj_set_style_text_color(title, lv_color_hex(0xDEB887), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
    
    // Lights status
    lights_label = lv_label_create(house_screen);
    lv_label_set_text(lights_label, "💡 Lights: 6/12 ON");
    lv_obj_set_style_text_color(lights_label, lv_color_hex(0xFFD700), 0);
    lv_obj_align(lights_label, LV_ALIGN_CENTER, 0, -25);
    
    // Thermostat
    thermostat_label = lv_label_create(house_screen);
    lv_label_set_text(thermostat_label, "🌡️ Temp: 21°C");
    lv_obj_set_style_text_color(thermostat_label, lv_color_hex(0x00CED1), 0);
    lv_obj_align(thermostat_label, LV_ALIGN_CENTER, 0, 0);
    
    // Security
    security_label = lv_label_create(house_screen);
    lv_label_set_text(security_label, "🔒 Security: ARMED");
    lv_obj_set_style_text_color(security_label, lv_color_hex(0x32CD32), 0);
    lv_obj_align(security_label, LV_ALIGN_CENTER, 0, 25);
    
    // Status
    status_label = lv_label_create(house_screen);
    lv_label_set_text(status_label, "All systems OK");
    lv_obj_set_style_text_color(status_label, lv_color_hex(0xDEB887), 0);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    ESP_LOGI(TAG, "House control screen initialized");
}

void house_screen_show(void) {
    if (house_screen) {
        lv_scr_load(house_screen);
        ESP_LOGI(TAG, "House control screen shown");
        
        // Update with mock data
        lv_label_set_text(lights_label, "💡 Lights: 6/12 ON");
        lv_label_set_text(thermostat_label, "🌡️ Temp: 21°C");
        lv_label_set_text(security_label, "🔒 Security: ARMED");
        lv_label_set_text(status_label, "All systems OK");
    }
}

void house_screen_handle_button_press(void) {
    ESP_LOGI(TAG, "House control button pressed - toggling devices");
    
    // Mock device state cycling
    static int device_state = 0;
    device_state = (device_state + 1) % 3;
    
    switch (device_state) {
        case 0:
            lv_label_set_text(lights_label, "💡 Lights: 6/12 ON");
            lv_label_set_text(thermostat_label, "🌡️ Temp: 21°C");
            lv_label_set_text(security_label, "🔒 Security: ARMED");
            lv_label_set_text(status_label, "All systems OK");
            break;
        case 1:
            lv_label_set_text(lights_label, "💡 Lights: 12/12 ON");
            lv_label_set_text(thermostat_label, "🌡️ Temp: 23°C");
            lv_label_set_text(security_label, "🔓 Security: DISARMED");
            lv_label_set_text(status_label, "Home mode active");
            break;
        case 2:
            lv_label_set_text(lights_label, "💡 Lights: 2/12 ON");
            lv_label_set_text(thermostat_label, "🌡️ Temp: 19°C");
            lv_label_set_text(security_label, "🔒 Security: NIGHT");
            lv_label_set_text(status_label, "Night mode active");
            break;
    }
}
