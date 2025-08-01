#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
// #include "ui.h"  // Uncomment this when you export from SquareLine Studio

// Display and LVGL setup
TFT_eSPI tft = TFT_eSPI();
static const uint16_t screenWidth = 360;
static const uint16_t screenHeight = 360;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

// WiFi and MQTT Management
WiFiManager wm;
WiFiClient espClient;
PubSubClient mqtt(espClient);

bool wifi_connected = false;
bool mqtt_connected = false;

// MQTT Configuration (will be set via WiFiManager)
String mqtt_server = "";
String mqtt_port = "1883";
String mqtt_username = "";
String mqtt_password = "";
String mqtt_client_id = "ESP32-Knob-";

// MQTT Topics to subscribe to
const char* topics[] = {
    "home/knob/command",
    "home/lighting/brightness",
    "home/audio/volume",
    "home/hvac/temperature"
};
const int num_topics = sizeof(topics) / sizeof(topics[0]);

// MQTT callback function
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    // Convert payload to string
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.printf("MQTT Received [%s]: %s\n", topic, message.c_str());
    
    // Handle different topics
    if (String(topic) == "home/knob/command") {
        // Handle knob commands
        if (message == "reset_wifi") {
            wm.resetSettings();
            ESP.restart();
        }
    } else if (String(topic) == "home/lighting/brightness") {
        // Handle lighting brightness
        int brightness = message.toInt();
        Serial.printf("Setting brightness to: %d\n", brightness);
        // TODO: Update UI to show brightness value
    } else if (String(topic) == "home/audio/volume") {
        // Handle audio volume
        int volume = message.toInt();
        Serial.printf("Setting volume to: %d\n", volume);
        // TODO: Update UI to show volume value
    } else if (String(topic) == "home/hvac/temperature") {
        // Handle temperature
        float temp = message.toFloat();
        Serial.printf("Temperature: %.1f°C\n", temp);
        // TODO: Update UI to show temperature
    }
}

// Connect to MQTT broker
bool connect_mqtt() {
    if (mqtt_server.length() == 0) {
        Serial.println("MQTT server not configured");
        return false;
    }
    
    mqtt.setServer(mqtt_server.c_str(), mqtt_port.toInt());
    mqtt.setCallback(mqtt_callback);
    
    // Generate unique client ID
    String clientId = mqtt_client_id + String(WiFi.macAddress());
    
    Serial.printf("Connecting to MQTT broker: %s:%s\n", mqtt_server.c_str(), mqtt_port.c_str());
    
    bool connected = false;
    if (mqtt_username.length() > 0 && mqtt_password.length() > 0) {
        // Connect with credentials
        connected = mqtt.connect(clientId.c_str(), mqtt_username.c_str(), mqtt_password.c_str());
    } else {
        // Connect without credentials
        connected = mqtt.connect(clientId.c_str());
    }
    
    if (connected) {
        Serial.println("MQTT Connected!");
        mqtt_connected = true;
        
        // Subscribe to all topics
        for (int i = 0; i < num_topics; i++) {
            if (mqtt.subscribe(topics[i])) {
                Serial.printf("Subscribed to: %s\n", topics[i]);
            } else {
                Serial.printf("Failed to subscribe to: %s\n", topics[i]);
            }
        }
        
        // Publish a status message
        mqtt.publish("home/knob/status", "online");
        
    } else {
        Serial.printf("MQTT Connection failed, state: %d\n", mqtt.state());
        mqtt_connected = false;
    }
    
    return connected;
}

// WiFi status callback
void wifi_status_callback() {
    if (WiFi.status() == WL_CONNECTED) {
        wifi_connected = true;
        Serial.println("WiFi Connected!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        
        // Try to connect to MQTT when WiFi connects
        if (mqtt_server.length() > 0) {
            connect_mqtt();
        }
    } else {
        wifi_connected = false;
        mqtt_connected = false;
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
    
    // Custom parameters for MQTT configuration
    WiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT Server", "", 40);
    WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", "1883", 6);
    WiFiManagerParameter custom_mqtt_username("mqtt_username", "MQTT Username", "", 32);
    WiFiManagerParameter custom_mqtt_password("mqtt_password", "MQTT Password", "", 32);
    WiFiManagerParameter custom_device_name("device", "Device Name", "ESP32-Knob", 20);
    
    // Add custom parameters to WiFi Manager
    wm.addParameter(&custom_mqtt_server);
    wm.addParameter(&custom_mqtt_port);
    wm.addParameter(&custom_mqtt_username);
    wm.addParameter(&custom_mqtt_password);
    wm.addParameter(&custom_device_name);
    
    Serial.println("Starting WiFi Manager...");
    
    // Try to connect, if it fails start config portal
    if (!wm.autoConnect("ESP32-Knob-Setup", "knob123")) {
        Serial.println("Failed to connect and hit timeout");
        // Could restart here or continue without WiFi
        ESP.restart();
    }
    
    // Connected! Save MQTT parameters
    mqtt_server = custom_mqtt_server.getValue();
    mqtt_port = custom_mqtt_port.getValue();
    mqtt_username = custom_mqtt_username.getValue();
    mqtt_password = custom_mqtt_password.getValue();
    
    // Add MAC address to client ID for uniqueness
    mqtt_client_id += String(WiFi.macAddress()).substring(9); // Last 3 pairs
    mqtt_client_id.replace(":", "");
    
    Serial.printf("MQTT Config - Server: %s, Port: %s, Username: %s\n", 
                  mqtt_server.c_str(), mqtt_port.c_str(), mqtt_username.c_str());
    
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
    
    // Create a label to show the status
    lv_obj_t *label = lv_label_create(lv_scr_act());
    String status_text = "ESP32 Knob\n\nWiFi: ";
    status_text += wifi_connected ? "Connected" : "Disconnected";
    status_text += "\nMQTT: ";
    status_text += mqtt_connected ? "Connected" : "Disconnected";
    
    if (wifi_connected) {
        status_text += "\nIP: " + WiFi.localIP().toString();
    }
    if (mqtt_connected && mqtt_server.length() > 0) {
        status_text += "\nBroker: " + mqtt_server;
    }
    
    lv_label_set_text(label, status_text.c_str());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(label);
    
    // Style the arc based on connection status
    lv_obj_set_style_arc_width(arc, 12, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 12, LV_PART_INDICATOR);
    
    lv_color_t arc_color;
    if (wifi_connected && mqtt_connected) {
        arc_color = lv_palette_main(LV_PALETTE_GREEN);  // Both connected
    } else if (wifi_connected) {
        arc_color = lv_palette_main(LV_PALETTE_ORANGE); // WiFi only
    } else {
        arc_color = lv_palette_main(LV_PALETTE_RED);    // No connection
    }
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
    // Handle LVGL tasks
    lv_timer_handler();
    
    // Handle WiFiManager portal
    wifiManager.process();
    
    // Update WiFi connection status
    bool current_wifi_status = WiFi.status() == WL_CONNECTED;
    if (current_wifi_status != wifi_connected) {
        wifi_connected = current_wifi_status;
        Serial.printf("WiFi status changed: %s\n", wifi_connected ? "Connected" : "Disconnected");
        ui_needs_update = true;
        
        // If WiFi disconnected, mark MQTT as disconnected too
        if (!wifi_connected && mqtt_connected) {
            mqtt_connected = false;
            ui_needs_update = true;
        }
    }
    
    // Handle MQTT connection if WiFi is connected
    if (wifi_connected) {
        // Check if MQTT client is connected
        bool current_mqtt_status = mqtt.connected();
        if (current_mqtt_status != mqtt_connected) {
            mqtt_connected = current_mqtt_status;
            Serial.printf("MQTT status changed: %s\n", mqtt_connected ? "Connected" : "Disconnected");
            ui_needs_update = true;
        }
        
        // Attempt to connect/reconnect to MQTT if not connected
        if (!mqtt_connected) {
            connect_mqtt();
        }
        
        // Process MQTT messages
        mqtt.loop();
    }
    
    // Update UI if needed
    if (ui_needs_update) {
        ui_needs_update = false;
        // Clear screen and recreate UI with updated status
        lv_obj_clean(lv_scr_act());
        create_demo_ui();
    }
    
    delay(10);  // Small delay to prevent watchdog issues
}
