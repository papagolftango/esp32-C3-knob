#include "clock_screen.h"
#include <esp_log.h>
#include <esp_sntp.h>
#include <time.h>
#include <sys/time.h>

static const char *TAG = "CLOCK_SCREEN";

static lv_obj_t *clock_screen;
static lv_obj_t *time_label;
static lv_obj_t *date_label;
static lv_obj_t *digital_clock;

void clock_screen_init(void) {
    ESP_LOGI(TAG, "Initializing clock screen");
    
    // Create screen object
    clock_screen = lv_obj_create(NULL);
    
    // Create title
    lv_obj_t *title = lv_label_create(clock_screen);
    lv_label_set_text(title, "Digital Clock");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
    
    // Create large time display
    time_label = lv_label_create(clock_screen);
    lv_label_set_text(time_label, "12:34:56");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0x00FF00), 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -20);
    
    // Create date display
    date_label = lv_label_create(clock_screen);
    lv_label_set_text(date_label, "Mon, Jan 01");
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_16, 0);
    lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 40);
    
    // Create analog clock (arc representation)
    digital_clock = lv_arc_create(clock_screen);
    lv_obj_set_size(digital_clock, 120, 120);
    lv_obj_align(digital_clock, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_arc_set_range(digital_clock, 0, 60);
    lv_arc_set_value(digital_clock, 30); // Will update with seconds
    
    // Add navigation hint
    lv_obj_t *nav_label = lv_label_create(clock_screen);
    lv_label_set_text(nav_label, "Press to toggle format");
    lv_obj_set_style_text_font(nav_label, &lv_font_montserrat_14, 0);
    lv_obj_align(nav_label, LV_ALIGN_BOTTOM_MID, 0, -5);
    
    // Update time initially
    clock_screen_update_time();
    
    ESP_LOGI(TAG, "Clock screen initialized");
}

void clock_screen_show(void) {
    ESP_LOGI(TAG, "Showing clock screen");
    lv_scr_load(clock_screen);
    
    // Update time when screen is shown
    clock_screen_update_time();
}

void clock_screen_update_time(void) {
    if (time_label == NULL || date_label == NULL) return;
    
    time_t now;
    struct tm timeinfo;
    
    time(&now);
    localtime_r(&now, &timeinfo);
    
    // Format time (24-hour format)
    char time_str[16];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", &timeinfo);
    lv_label_set_text(time_label, time_str);
    
    // Format date
    char date_str[32];
    strftime(date_str, sizeof(date_str), "%a, %b %d", &timeinfo);
    lv_label_set_text(date_label, date_str);
    
    // Update arc with seconds (0-59)
    if (digital_clock != NULL) {
        lv_arc_set_value(digital_clock, timeinfo.tm_sec);
    }
}

void clock_screen_handle_button_press(void) {
    ESP_LOGI(TAG, "Button pressed on clock screen - toggling time format");
    
    // TODO: Toggle between 12/24 hour format
    // For now, just update the time
    clock_screen_update_time();
}
