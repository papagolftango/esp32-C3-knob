#include "weather_screen.h"
#include "esp_log.h"

static const char* TAG = "weather_screen";
static lv_obj_t* weather_screen = NULL;
static lv_obj_t* temp_label = NULL;
static lv_obj_t* condition_label = NULL;
static lv_obj_t* humidity_label = NULL;
static lv_obj_t* icon_label = NULL;

void weather_screen_init(void) {
    // Create screen
    weather_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(weather_screen, lv_color_hex(0x001144), 0);
    
    // Title
    lv_obj_t* title = lv_label_create(weather_screen);
    lv_label_set_text(title, "🌤️ WEATHER");
    lv_obj_set_style_text_color(title, lv_color_hex(0x87CEEB), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Weather icon/emoji
    icon_label = lv_label_create(weather_screen);
    lv_label_set_text(icon_label, "☀️");
    lv_obj_set_style_text_font(icon_label, &lv_font_montserrat_18, 0);
    lv_obj_align(icon_label, LV_ALIGN_CENTER, -40, -30);
    
    // Temperature
    temp_label = lv_label_create(weather_screen);
    lv_label_set_text(temp_label, "22°C");
    lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(temp_label, lv_color_hex(0xFF6B35), 0);
    lv_obj_align(temp_label, LV_ALIGN_CENTER, 20, -30);
    
    // Condition
    condition_label = lv_label_create(weather_screen);
    lv_label_set_text(condition_label, "Sunny");
    lv_obj_set_style_text_color(condition_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(condition_label, LV_ALIGN_CENTER, 0, 0);
    
    // Humidity
    humidity_label = lv_label_create(weather_screen);
    lv_label_set_text(humidity_label, "Humidity: 65%");
    lv_obj_set_style_text_color(humidity_label, lv_color_hex(0x87CEEB), 0);
    lv_obj_align(humidity_label, LV_ALIGN_CENTER, 0, 25);
    
    ESP_LOGI(TAG, "Weather screen initialized");
}

void weather_screen_show(void) {
    if (weather_screen) {
        lv_scr_load(weather_screen);
        ESP_LOGI(TAG, "Weather screen shown");
        
        // Update with mock data
        lv_label_set_text(icon_label, "☀️");
        lv_label_set_text(temp_label, "22°C");
        lv_label_set_text(condition_label, "Sunny");
        lv_label_set_text(humidity_label, "Humidity: 65%");
    }
}

void weather_screen_handle_button_press(void) {
    ESP_LOGI(TAG, "Weather screen button pressed - updating forecast");
    
    // Mock weather data cycling
    static int weather_state = 0;
    weather_state = (weather_state + 1) % 4;
    
    switch (weather_state) {
        case 0:
            lv_label_set_text(icon_label, "☀️");
            lv_label_set_text(temp_label, "22°C");
            lv_label_set_text(condition_label, "Sunny");
            lv_label_set_text(humidity_label, "Humidity: 65%");
            break;
        case 1:
            lv_label_set_text(icon_label, "☁️");
            lv_label_set_text(temp_label, "18°C");
            lv_label_set_text(condition_label, "Cloudy");
            lv_label_set_text(humidity_label, "Humidity: 78%");
            break;
        case 2:
            lv_label_set_text(icon_label, "🌧️");
            lv_label_set_text(temp_label, "15°C");
            lv_label_set_text(condition_label, "Rainy");
            lv_label_set_text(humidity_label, "Humidity: 89%");
            break;
        case 3:
            lv_label_set_text(icon_label, "❄️");
            lv_label_set_text(temp_label, "-2°C");
            lv_label_set_text(condition_label, "Snow");
            lv_label_set_text(humidity_label, "Humidity: 92%");
            break;
    }
}
