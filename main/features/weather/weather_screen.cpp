#include "weather_screen.h"
#include "weather_data_manager.h"
#include "esp_log.h"

static const char* TAG = "weather_screen";
static lv_obj_t* weather_screen = NULL;
static lv_obj_t* temp_label = NULL;
static lv_obj_t* condition_label = NULL;
static lv_obj_t* humidity_label = NULL;
static lv_obj_t* frost_warning_label = NULL;
static lv_obj_t* icon_label = NULL;

void weather_screen_init(void) {
    // Initialise weather data manager
    WeatherDataManager::init();
    
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
    lv_obj_align(icon_label, LV_ALIGN_CENTER, -40, -40);
    
    // Temperature
    temp_label = lv_label_create(weather_screen);
    lv_label_set_text(temp_label, WeatherDataManager::getTemperatureString().c_str());
    lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(temp_label, lv_color_hex(0xFF6B35), 0);
    lv_obj_align(temp_label, LV_ALIGN_CENTER, 20, -40);
    
    // Humidity
    humidity_label = lv_label_create(weather_screen);
    lv_label_set_text(humidity_label, ("Humidity: " + WeatherDataManager::getHumidityString()).c_str());
    lv_obj_set_style_text_color(humidity_label, lv_color_hex(0x87CEEB), 0);
    lv_obj_align(humidity_label, LV_ALIGN_CENTER, 0, -10);
    
    // Frost warning label
    frost_warning_label = lv_label_create(weather_screen);
    lv_label_set_text(frost_warning_label, WeatherDataManager::getFrostRiskString().c_str());
    lv_obj_set_style_text_color(frost_warning_label, 
        WeatherDataManager::getFrostRisk() ? lv_color_hex(0xFF4444) : lv_color_hex(0x44FF44), 0);
    lv_obj_align(frost_warning_label, LV_ALIGN_CENTER, 0, 15);
    
    // General condition (placeholder for now)
    condition_label = lv_label_create(weather_screen);
    lv_label_set_text(condition_label, "Conditions Good");
    lv_obj_set_style_text_color(condition_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(condition_label, LV_ALIGN_CENTER, 0, 40);
    
    ESP_LOGI(TAG, "Weather screen initialised");
}

void weather_screen_show(void) {
    if (weather_screen) {
        lv_scr_load(weather_screen);
        ESP_LOGI(TAG, "Weather screen shown");
        
        // Update display with current weather data
        updateWeatherDisplay();
    }
}

void updateWeatherDisplay(void) {
    if (!weather_screen) return;
    
    // Update temperature
    lv_label_set_text(temp_label, WeatherDataManager::getTemperatureString().c_str());
    
    // Update humidity
    lv_label_set_text(humidity_label, ("Humidity: " + WeatherDataManager::getHumidityString()).c_str());
    
    // Update frost warning with colour coding
    lv_label_set_text(frost_warning_label, WeatherDataManager::getFrostRiskString().c_str());
    lv_obj_set_style_text_color(frost_warning_label, 
        WeatherDataManager::getFrostRisk() ? lv_color_hex(0xFF4444) : lv_color_hex(0x44FF44), 0);
    
    // Clear change flags
    WeatherDataManager::clearChangeFlags();
    
    ESP_LOGI(TAG, "Weather display updated: %s, %s, Frost: %s",
        WeatherDataManager::getTemperatureString().c_str(),
        WeatherDataManager::getHumidityString().c_str(),
        WeatherDataManager::getFrostRisk() ? "YES" : "NO");
}

void weather_screen_handle_button_press(void) {
    ESP_LOGI(TAG, "Weather screen button pressed - cycling test data");
    
    // Mock weather data cycling for testing
    static int weather_state = 0;
    weather_state = (weather_state + 1) % 4;
    
    switch (weather_state) {
        case 0:
            WeatherDataManager::updateTemperature(22.3f);
            WeatherDataManager::updateHumidity(65.0f);
            WeatherDataManager::updateFrostRisk(false);
            lv_label_set_text(icon_label, "☀️");
            lv_label_set_text(condition_label, "Sunny");
            break;
        case 1:
            WeatherDataManager::updateTemperature(18.1f);
            WeatherDataManager::updateHumidity(78.5f);
            WeatherDataManager::updateFrostRisk(false);
            lv_label_set_text(icon_label, "☁️");
            lv_label_set_text(condition_label, "Cloudy");
            break;
        case 2:
            WeatherDataManager::updateTemperature(15.8f);
            WeatherDataManager::updateHumidity(89.2f);
            WeatherDataManager::updateFrostRisk(false);
            lv_label_set_text(icon_label, "🌧️");
            lv_label_set_text(condition_label, "Rainy");
            break;
        case 3:
            WeatherDataManager::updateTemperature(-1.5f);
            WeatherDataManager::updateHumidity(92.0f);
            WeatherDataManager::updateFrostRisk(true);
            lv_label_set_text(icon_label, "❄️");
            lv_label_set_text(condition_label, "Frost Risk");
            break;
    }
    
    // Update display with new data
    updateWeatherDisplay();
}
