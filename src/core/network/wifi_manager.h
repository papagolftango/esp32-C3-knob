#pragma once

#include <WiFi.h>
#include <WiFiManager.h>

class WiFiManagerWrapper {
public:
    // Initialize WiFi management
    static void begin();
    
    // Setup WiFi with portal for configuration
    static void setupWiFi();
    
    // Process WiFi manager portal
    static void process();
    
    // Check if WiFi is connected
    static bool isConnected();
    
    // Get connection status change (for UI updates)
    static bool hasStatusChanged();
    
    // Reset WiFi settings (for settings menu)
    static void reset();
    
private:
    static WiFiManager wm;
    static bool wifi_connected;
    static bool last_wifi_status;
    static bool status_changed;
    
    static void updateConnectionStatus();
};
