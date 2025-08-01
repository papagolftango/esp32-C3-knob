#include "wifi_manager.h"

// Static member definitions
WiFiManager WiFiManagerWrapper::wm;
bool WiFiManagerWrapper::wifi_connected = false;
bool WiFiManagerWrapper::last_wifi_status = false;
bool WiFiManagerWrapper::status_changed = false;

void WiFiManagerWrapper::begin() {
    wifi_connected = false;
    last_wifi_status = false;
    status_changed = false;
}

void WiFiManagerWrapper::setupWiFi() {
    // WiFiManager setup
    wm.setAPCallback([](WiFiManager *myWiFiManager) {
        Serial.println("Entered config mode");
        Serial.println(WiFi.softAPIP());
        Serial.println(myWiFiManager->getConfigPortalSSID());
    });
    
    wm.setSaveConfigCallback([]() {
        Serial.println("Should save config");
    });
    
    // Set timeout for configuration portal
    wm.setConfigPortalTimeout(300); // 5 minutes
    
    // Auto connect or start configuration portal
    String ap_name = "ESP32-Knob-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    
    if (!wm.autoConnect(ap_name.c_str())) {
        Serial.println("Failed to connect and hit timeout");
        delay(3000);
        ESP.restart();
    }
    
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    wifi_connected = true;
    status_changed = true;
}

void WiFiManagerWrapper::process() {
    wm.process();
    updateConnectionStatus();
}

bool WiFiManagerWrapper::isConnected() {
    return wifi_connected;
}

bool WiFiManagerWrapper::hasStatusChanged() {
    if (status_changed) {
        status_changed = false;
        return true;
    }
    return false;
}

void WiFiManagerWrapper::reset() {
    wm.resetSettings();
    Serial.println("WiFi settings reset - restarting...");
    delay(1000);
    ESP.restart();
}

void WiFiManagerWrapper::updateConnectionStatus() {
    bool current_status = (WiFi.status() == WL_CONNECTED);
    
    if (current_status != last_wifi_status) {
        wifi_connected = current_status;
        last_wifi_status = current_status;
        status_changed = true;
        
        Serial.printf("WiFi status changed: %s\n", wifi_connected ? "Connected" : "Disconnected");
    }
}
