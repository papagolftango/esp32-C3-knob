#pragma once

#include <esp_wifi.h>
#include <esp_event.h>
#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>
#include <esp_log.h>
#include <string>

class WiFiManager {
public:
    WiFiManager();
    ~WiFiManager();
    
    // Initialize WiFi and start provisioning if needed
    esp_err_t initialize();
    
    // Check if device is provisioned (has WiFi credentials)
    bool isProvisioned();
    
    // Start provisioning process (captive portal/BLE)
    esp_err_t startProvisioning();
    
    // Stop provisioning and connect to saved network
    esp_err_t stopProvisioning();
    
    // Get current WiFi status
    bool isConnected();
    
    // Reset WiFi credentials (for factory reset)
    esp_err_t resetCredentials();
    
    // Callbacks
    typedef void (*wifi_connected_cb_t)();
    typedef void (*wifi_disconnected_cb_t)();
    typedef void (*provisioning_complete_cb_t)();
    
    void setCallbacks(wifi_connected_cb_t connected_cb, 
                     wifi_disconnected_cb_t disconnected_cb,
                     provisioning_complete_cb_t prov_complete_cb);

private:
    static const char* TAG;
    bool _initialized;
    bool _provisioned;
    bool _connected;
    
    // Callbacks
    wifi_connected_cb_t _connected_cb;
    wifi_disconnected_cb_t _disconnected_cb;
    provisioning_complete_cb_t _prov_complete_cb;
    
    // Event handlers
    static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                  int32_t event_id, void* event_data);
    static void prov_event_handler(void* arg, esp_event_base_t event_base,
                                  int32_t event_id, void* event_data);
    
    // Helper methods
    esp_err_t initWiFi();
    esp_err_t startSoftAP();
};
