#include "hello_screen.h"
#include <esp_log.h>

static const char *TAG = "HELLO_SCREEN";

static lv_obj_t *hello_screen;
static lv_obj_t *title_label;
static lv_obj_t *wifi_label;
static lv_obj_t *arc_widget;

void hello_screen_init(void) {
    ESP_LOGI(TAG, "Initializing hello world screen");
    
    // Create screen object
    hello_screen = lv_obj_create(NULL);
    
    // Create title label
    title_label = lv_label_create(hello_screen);
    lv_label_set_text(title_label, "ESP32-S3 Knob\nESP-IDF + LVGL\n+ WiFiManager");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_16, 0);
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, -60);
    
    // Create WiFi status label
    wifi_label = lv_label_create(hello_screen);
    lv_label_set_text(wifi_label, "WiFi: Initializing...");
    lv_obj_set_style_text_font(wifi_label, &lv_font_montserrat_18, 0);
    lv_obj_align(wifi_label, LV_ALIGN_CENTER, 0, 0);
    
    // Create an arc widget
    arc_widget = lv_arc_create(hello_screen);
    lv_obj_set_size(arc_widget, 150, 150);
    lv_obj_align(arc_widget, LV_ALIGN_CENTER, 0, 60);
    lv_arc_set_value(arc_widget, 50);
    
    // Add navigation hint
    lv_obj_t *nav_label = lv_label_create(hello_screen);
    lv_label_set_text(nav_label, "Rotate knob to navigate");
    lv_obj_set_style_text_font(nav_label, &lv_font_montserrat_14, 0);
    lv_obj_align(nav_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    ESP_LOGI(TAG, "Hello world screen initialized");
}

void hello_screen_show(void) {
    ESP_LOGI(TAG, "Showing hello world screen");
    lv_scr_load(hello_screen);
}

void hello_screen_update_wifi_status(bool connected, bool provisioning) {
    if (wifi_label == NULL) return;
    
    if (provisioning) {
        lv_label_set_text(wifi_label, "WiFi: Provisioning...\nUse ESP WiFi Prov app");
        lv_obj_set_style_text_color(wifi_label, lv_color_hex(0xFFA500), 0); // Orange
    } else if (connected) {
        lv_label_set_text(wifi_label, "WiFi: Connected");
        lv_obj_set_style_text_color(wifi_label, lv_color_hex(0x00FF00), 0); // Green
    } else {
        lv_label_set_text(wifi_label, "WiFi: Disconnected");
        lv_obj_set_style_text_color(wifi_label, lv_color_hex(0xFF0000), 0); // Red
    }
}
