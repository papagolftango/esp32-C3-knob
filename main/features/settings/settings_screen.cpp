#include "settings_screen.h"
#include <esp_log.h>

static const char *TAG = "SETTINGS_SCREEN";

static lv_obj_t *settings_screen;
static lv_obj_t *title_label;
static lv_obj_t *settings_list;

void settings_screen_init(void) {
    ESP_LOGI(TAG, "Initializing settings screen");
    
    // Create screen object
    settings_screen = lv_obj_create(NULL);
    
    // Create title label
    title_label = lv_label_create(settings_screen);
    lv_label_set_text(title_label, "Settings");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 20);
    
    // Create settings list
    settings_list = lv_list_create(settings_screen);
    lv_obj_set_size(settings_list, 280, 200);
    lv_obj_align(settings_list, LV_ALIGN_CENTER, 0, 20);
    
    // Add some example settings items
    lv_obj_t *btn;
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_WIFI, "WiFi Reset");
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_SETTINGS, "Display Brightness");
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_VOLUME_MAX, "Haptic Feedback");
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_REFRESH, "Factory Reset");
    
    // Add navigation hint
    lv_obj_t *nav_label = lv_label_create(settings_screen);
    lv_label_set_text(nav_label, "Rotate knob to navigate");
    lv_obj_set_style_text_font(nav_label, &lv_font_montserrat_14, 0);
    lv_obj_align(nav_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    ESP_LOGI(TAG, "Settings screen initialized");
}

void settings_screen_show(void) {
    ESP_LOGI(TAG, "Showing settings screen");
    lv_scr_load(settings_screen);
}

void settings_screen_handle_button_press(void) {
    ESP_LOGI(TAG, "Button pressed on settings screen");
    // TODO: Implement settings item selection/activation
}
