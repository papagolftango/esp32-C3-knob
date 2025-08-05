#include "wifi_manager.h"
#include <esp_netif.h>
#include <esp_log.h>
#include <string.h>

const char* WiFiManager::TAG = "WiFiManager";

WiFiManager::WiFiManager() : 
    _initialized(false), 
    _provisioned(false), 
    _connected(false),
    _connected_cb(nullptr),
    _disconnected_cb(nullptr),
    _prov_complete_cb(nullptr) {
}

WiFiManager::~WiFiManager() {
    if (_initialized) {
        wifi_prov_mgr_deinit();
    }
}

esp_err_t WiFiManager::initialize() {
    if (_initialized) {
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Initializing WiFi Manager");
    
    // Initialize network interface
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // Create default WiFi AP and STA
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();
    
    // Initialize WiFi
    ESP_ERROR_CHECK(initWiFi());
    
    // Initialize provisioning manager
    wifi_prov_mgr_config_t config = {
        .scheme = wifi_prov_scheme_ble,
        .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
    };
    
    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));
    
    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, 
                                              &wifi_event_handler, this));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, 
                                              &prov_event_handler, this));
    
    // Check if device is already provisioned
    wifi_prov_mgr_is_provisioned(&_provisioned);
    
    _initialized = true;
    ESP_LOGI(TAG, "WiFi Manager initialized. Provisioned: %s", _provisioned ? "Yes" : "No");
    
    return ESP_OK;
}

bool WiFiManager::isProvisioned() {
    if (!_initialized) {
        return false;
    }
    
    bool provisioned = false;
    wifi_prov_mgr_is_provisioned(&provisioned);
    return provisioned;
}

esp_err_t WiFiManager::startProvisioning() {
    if (!_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (isProvisioned()) {
        ESP_LOGI(TAG, "Already provisioned, starting WiFi");
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Starting provisioning process");
    
    // Start provisioning with BLE
    const char* service_name = "ESP32_Knob";
    const char* service_key = nullptr; // No security for simplicity
    
    ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(WIFI_PROV_SECURITY_1, 
                                                    nullptr, service_name, service_key));
    
    ESP_LOGI(TAG, "Provisioning started. Device name: %s", service_name);
    ESP_LOGI(TAG, "Use ESP WiFi Provisioning app to configure");
    
    return ESP_OK;
}

esp_err_t WiFiManager::stopProvisioning() {
    if (!_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    wifi_prov_mgr_stop_provisioning();
    return ESP_OK;
}

bool WiFiManager::isConnected() {
    return _connected;
}

esp_err_t WiFiManager::resetCredentials() {
    if (!_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    ESP_LOGI(TAG, "Resetting WiFi credentials");
    wifi_prov_mgr_reset_provisioning();
    _provisioned = false;
    _connected = false;
    
    return ESP_OK;
}

void WiFiManager::setCallbacks(wifi_connected_cb_t connected_cb, 
                              wifi_disconnected_cb_t disconnected_cb,
                              provisioning_complete_cb_t prov_complete_cb) {
    _connected_cb = connected_cb;
    _disconnected_cb = disconnected_cb;
    _prov_complete_cb = prov_complete_cb;
}

esp_err_t WiFiManager::initWiFi() {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    
    return ESP_OK;
}

void WiFiManager::wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data) {
    WiFiManager* manager = static_cast<WiFiManager*>(arg);
    
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WiFi STA started");
            esp_wifi_connect();
            break;
            
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "WiFi STA connected");
            break;
            
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "WiFi STA disconnected");
            manager->_connected = false;
            if (manager->_disconnected_cb) {
                manager->_disconnected_cb();
            }
            // Try to reconnect
            esp_wifi_connect();
            break;
            
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "WiFi connected - got IP address");
            manager->_connected = true;
            if (manager->_connected_cb) {
                manager->_connected_cb();
            }
            break;
    }
}

void WiFiManager::prov_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data) {
    WiFiManager* manager = static_cast<WiFiManager*>(arg);
    
    switch (event_id) {
        case WIFI_PROV_START:
            ESP_LOGI(TAG, "Provisioning started");
            break;
            
        case WIFI_PROV_CRED_RECV: {
            wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
            ESP_LOGI(TAG, "Received WiFi credentials - SSID: %s", (const char *)wifi_sta_cfg->ssid);
            break;
        }
        
        case WIFI_PROV_CRED_SUCCESS:
            ESP_LOGI(TAG, "Provisioning successful");
            manager->_provisioned = true;
            if (manager->_prov_complete_cb) {
                manager->_prov_complete_cb();
            }
            break;
            
        case WIFI_PROV_CRED_FAIL:
            ESP_LOGE(TAG, "Provisioning failed");
            break;
            
        case WIFI_PROV_END:
            ESP_LOGI(TAG, "Provisioning ended");
            wifi_prov_mgr_deinit();
            break;
    }
}
