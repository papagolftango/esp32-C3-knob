#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <Wire.h>
// #include "ui.h"  // Uncomment this when you export from SquareLine Studio

// Refactored components
#include "core/hardware/haptic_feedback.h"
#include "core/hardware/rotary_encoder.h"
#include "core/network/wifi_manager.h"
#include "core/network/mqtt_manager.h"
#include "features/energy/energy_ui.h"
#include "features/energy/energy_data.h"
#include "features/settings/settings_ui.h"

// Display and LVGL setup
TFT_eSPI tft = TFT_eSPI();
static const uint16_t screenWidth = 360;
static const uint16_t screenHeight = 360;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

bool ui_needs_update = true;

// Screen Management
Screen current_screen = SCREEN_ENERGY;  // Default to Energy
const char* screen_names[] = {
    "Energy",
    "Weather", 
    "House Info",
    "Settings"
};

// Navigation state
unsigned long last_interaction = 0;
bool screen_changed = true;

// Forward declarations
void next_screen();
void previous_screen();

// Navigation callback for rotary encoder
void on_navigation_change(int direction) {
    if (current_screen == SCREEN_SETTINGS && SettingsUI::isMenuActive()) {
        // Navigate settings menu
        SettingsUI::handleEncoderRotation(direction);
        screen_changed = true;  // Trigger screen update
    } else {
        // Normal screen navigation
        if (direction > 0) {
            // Clockwise - next screen
            next_screen();
            HapticFeedback::screenChange();  // Strong haptic feedback for screen change
            Serial.println("Rotary: Next screen (CW)");
        } else {
            // Counter-clockwise - previous screen
            previous_screen();
            HapticFeedback::screenChange();  // Strong haptic feedback for screen change
            Serial.println("Rotary: Previous screen (CCW)");
        }
    }
}

// Touch handling
bool touch_has_signal(void);
bool touch_touched(void);
bool touch_released(void);
void touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

// Display flushing
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

// Touch input reading
void touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;
    // For ESP32-S3 board, touch might not be available through TFT_eSPI
    // Using a simple approach - disable touch for now
    bool touched = false; // tft.getTouch(&touchX, &touchY);

    if (touched) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// Screen creation functions
// Energy screen functionality moved to features/energy/energy_ui.h

void create_weather_screen(void)
{
    // Create title
    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "🌤️ WEATHER");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
    
    // Temperature arc
    lv_obj_t *arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(arc, 180, 180);
    lv_obj_center(arc);
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_arc_set_value(arc, 72);  // Example: 72% of temp range
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    
    // Weather info
    lv_obj_t *weather_label = lv_label_create(lv_scr_act());
    lv_label_set_text(weather_label, "22°C\nPartly Cloudy\n45% Humidity");
    lv_obj_set_style_text_font(weather_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(weather_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(weather_label);
    
    // Data source
    lv_obj_t *source = lv_label_create(lv_scr_act());
    String source_text = MQTTManager::isConnected() ? "📡 Live Weather" : "📡 Offline";
    lv_label_set_text(source, source_text.c_str());
    lv_obj_align(source, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    // Arc color for temperature
    lv_color_t arc_color = lv_palette_main(LV_PALETTE_ORANGE);
    lv_obj_set_style_arc_color(arc, arc_color, LV_PART_INDICATOR);
}

void create_house_info_screen(void)
{
    // Create title
    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "🏠 HOUSE INFO");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
    
    // Connection status arc
    lv_obj_t *arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(arc, 180, 180);
    lv_obj_center(arc);
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    int connection_pct = (wifiManager.isConnected() ? 50 : 0) + (mqttManager.isConnected() ? 50 : 0);
    lv_arc_set_value(arc, connection_pct);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    
    // House systems info
    lv_obj_t *info_label = lv_label_create(lv_scr_act());
    String info_text = "WiFi: ";
    info_text += wifiManager.isConnected() ? "✅" : "❌";
    info_text += "\nMQTT: ";
    info_text += mqttManager.isConnected() ? "✅" : "❌";
    if (wifiManager.isConnected()) {
        info_text += "\nIP: " + WiFi.localIP().toString();
    }
    lv_label_set_text(info_label, info_text.c_str());
    lv_obj_set_style_text_font(info_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(info_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(info_label);
    
    // Device info
    lv_obj_t *device = lv_label_create(lv_scr_act());
    lv_label_set_text(device, "ESP32-C3 Knob");
    lv_obj_align(device, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    // Arc color based on connection
    lv_color_t arc_color;
    if (wifiManager.isConnected() && mqttManager.isConnected()) {
        arc_color = lv_palette_main(LV_PALETTE_GREEN);
    } else if (wifiManager.isConnected()) {
        arc_color = lv_palette_main(LV_PALETTE_ORANGE);
    } else {
        arc_color = lv_palette_main(LV_PALETTE_RED);
    }
    lv_obj_set_style_arc_color(arc, arc_color, LV_PART_INDICATOR);
}

// Navigation functions
void switch_to_screen(Screen new_screen)
{
    if (new_screen != current_screen) {
        // Reset settings menu when leaving settings screen
        if (current_screen == SCREEN_SETTINGS) {
            SettingsUI::reset();
        }
        
        current_screen = new_screen;
        screen_changed = true;
        ui_needs_update = true;
        last_interaction = millis();
        
        Serial.printf("Switched to screen: %s\n", screen_names[current_screen]);
    }
}

void next_screen()
{
    Screen next = (Screen)((current_screen + 1) % SCREEN_COUNT);
    switch_to_screen(next);
}

void previous_screen()
{
    Screen prev = (Screen)((current_screen - 1 + SCREEN_COUNT) % SCREEN_COUNT);
    switch_to_screen(prev);
}

// Update current screen
void update_current_screen(void)
{
    // Clear screen
    lv_obj_clean(lv_scr_act());
    
    // Create the appropriate screen
    switch (current_screen) {
        case SCREEN_ENERGY:
            EnergyUI::updateScreen(EnergyData_Manager::getCurrentData(), EnergyData_Manager::getPeakData());
            break;
        case SCREEN_WEATHER:
            create_weather_screen();
            break;
        case SCREEN_HOUSE_INFO:
            create_house_info_screen();
            break;
        case SCREEN_SETTINGS:
            SettingsUI::updateScreen();
            break;
    }
    
    // Add screen indicator at top
    lv_obj_t *indicator = lv_label_create(lv_scr_act());
    String indicator_text = String(current_screen + 1) + "/" + String(SCREEN_COUNT) + " - Rotate to change";
    lv_label_set_text(indicator, indicator_text.c_str());
    lv_obj_set_style_text_font(indicator, &lv_font_montserrat_10, 0);
    lv_obj_align(indicator, LV_ALIGN_TOP_MID, 0, 5);
}

// Legacy function - now calls update_current_screen
void create_demo_ui(void)
{
    update_current_screen();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting ESP32-C3 Knob Display...");

    // Initialize network management
    WiFiManagerWrapper::begin();
    MQTTManager::begin();
    
    // Setup WiFi with configuration portal
    WiFiManagerWrapper::setupWiFi();
    
    // Configure MQTT (using default settings)
    MQTTManager::configure("192.168.1.100", 1883, "", "");

    // Initialize display
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    // Initialize touch calibration
    uint16_t calData[5] = {275, 3620, 264, 3532, 1};
    tft.setTouch(calData);

    // Initialize LVGL
    lv_init();

    // Initialize display buffer
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

    // Initialize display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Initialize input device driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read;
    lv_indev_drv_register(&indev_drv);

    // Initialize rotary encoder
    RotaryEncoder::begin();
    RotaryEncoder::setNavigationCallback(on_navigation_change);

    // Initialize haptic feedback
    HapticFeedback::begin();
    
    // Initialize energy data manager
    EnergyData_Manager::begin();
    
    // Initialize settings UI
    SettingsUI::begin();
    
    // Play startup confirmation
    HapticFeedback::confirmation();

    // Create the demo UI (will be replaced by SquareLine Studio)
    create_demo_ui();
    
    // When using SquareLine Studio, replace the above with:
    // ui_init();  // This will be generated by SquareLine Studio

    Serial.println("Setup complete!");
}

void loop()
{
    // Update energy data
    EnergyData_Manager::update();
    
    // Handle LVGL tasks
    lv_timer_handler();
    
    // Handle WiFiManager portal
    WiFiManagerWrapper::process();
    
    // Handle rotary encoder navigation
    RotaryEncoder::handleNavigation();
    
    // Touch screen navigation (primary button replacement)
    // Touch screen to enter/confirm actions
    static bool touch_was_pressed = false;
    static unsigned long last_touch_time = 0;
    
    // Check for touch events
    lv_indev_t * indev = lv_indev_get_next(NULL);
    if (indev != NULL) {
        lv_indev_data_t data;
        lv_indev_read(indev, &data);
        
        bool touch_pressed = (data.state == LV_INDEV_STATE_PR);
        unsigned long now = millis();
        
        // Touch acts as "button press" - could be used for settings or actions
        if (touch_pressed && !touch_was_pressed && (now - last_touch_time > 400)) {
            if (current_screen == SCREEN_SETTINGS) {
                SettingsUI::handleSelection();
                screen_changed = true;
            } else {
                // Normal screen navigation
                HapticFeedback::screenChange();
                next_screen();
            }
            last_touch_time = now;
        }
        touch_was_pressed = touch_pressed;
    }
    
    // Handle network connections
    bool wifi_status_changed = WiFiManagerWrapper::hasStatusChanged();
    bool mqtt_status_changed = MQTTManager::hasStatusChanged();
    
    if (wifi_status_changed || mqtt_status_changed) {
        ui_needs_update = true;
    }
    
    // Handle MQTT connection if WiFi is connected
    if (WiFiManagerWrapper::isConnected()) {
        // Attempt to connect/reconnect to MQTT if not connected
        if (!MQTTManager::isConnected()) {
            MQTTManager::connect();
        }
        
        // Process MQTT messages
        MQTTManager::process();
    }
    
    // Update UI if needed (screen changed or connection status changed)
    if (ui_needs_update || screen_changed) {
        ui_needs_update = false;
        screen_changed = false;
        update_current_screen();
    }
    
    delay(10);  // Small delay to prevent watchdog issues
}
