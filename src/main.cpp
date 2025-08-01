#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include <WiFi.h>
#include <WiFiManager.h>
// #include "ui.h"  // Uncomment this when you export from SquareLine Studio

// Display and LVGL setup
TFT_eSPI tft = TFT_eSPI();
static const uint16_t screenWidth = 360;
static const uint16_t screenHeight = 360;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

// WiFi Management
WiFiManager wm;
bool wifi_connected = false;

// WiFi status callback
void wifi_status_callback() {
    if (WiFi.status() == WL_CONNECTED) {
        wifi_connected = true;
        Serial.println("WiFi Connected!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    } else {
        wifi_connected = false;
        Serial.println("WiFi Disconnected");
    }
}

// Setup WiFi with captive portal
void setup_wifi() {
    // Reset settings for testing (comment out in production)
    // wm.resetSettings();
    
    // Set custom AP name and password for config portal
    wm.setAPStaticIPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
    
    // Configure captive portal timeout
    wm.setConfigPortalTimeout(300); // 5 minutes timeout
    
    // Custom parameters (you can add device name, etc.)
    WiFiManagerParameter custom_device_name("device", "Device Name", "ESP32-Knob", 20);
    wm.addParameter(&custom_device_name);
    
    Serial.println("Starting WiFi Manager...");
    
    // Try to connect, if it fails start config portal
    if (!wm.autoConnect("ESP32-Knob-Setup", "knob123")) {
        Serial.println("Failed to connect and hit timeout");
        // Could restart here or continue without WiFi
        ESP.restart();
    }
    
    // Connected!
    wifi_status_callback();
    
    // Set up WiFi event handlers
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        wifi_status_callback();
    }, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        wifi_status_callback();
    }, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
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
    bool touched = tft.getTouch(&touchX, &touchY);

    if (touched) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// Create a simple knob widget demo (TEMPORARY - will be replaced by SquareLine Studio UI)
void create_demo_ui(void)
{
    // Create a knob (arc widget)
    lv_obj_t *arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(arc, 200, 200);  // Larger for 360x360 screen
    lv_obj_center(arc);
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);   // Remove the knob of the arc
    
    // Create a label to show the value
    lv_obj_t *label = lv_label_create(lv_scr_act());
    String status_text = "SquareLine\nStudio Ready\n\nWiFi: ";
    status_text += wifi_connected ? "Connected" : "Disconnected";
    if (wifi_connected) {
        status_text += "\nIP: " + WiFi.localIP().toString();
    }
    lv_label_set_text(label, status_text.c_str());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(label);
    
    // Style the arc - green if WiFi connected, red if not
    lv_obj_set_style_arc_width(arc, 12, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 12, LV_PART_INDICATOR);
    lv_color_t arc_color = wifi_connected ? lv_palette_main(LV_PALETTE_GREEN) : lv_palette_main(LV_PALETTE_RED);
    lv_obj_set_style_arc_color(arc, arc_color, LV_PART_INDICATOR);
    
    // This temporary UI will be replaced by SquareLine Studio generated code
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting ESP32-C3 Knob Display...");

    // Setup WiFi first
    setup_wifi();

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

    // Create the demo UI (will be replaced by SquareLine Studio)
    create_demo_ui();
    
    // When using SquareLine Studio, replace the above with:
    // ui_init();  // This will be generated by SquareLine Studio

    Serial.println("Setup complete!");
}

void loop()
{
    lv_timer_handler(); // Handle LVGL tasks
    delay(5);
}
