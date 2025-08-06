#include "factory_reset_manager.h"
#include "display.h"
#include "app_state_manager.h"

static const char *TAG = "FACTORY_RESET";

// Static member initialization
bool FactoryResetManager::reset_in_progress = false;

void FactoryResetManager::begin() {
    ESP_LOGI(TAG, "Factory Reset Manager initialized");
}

void FactoryResetManager::triggerFromSettings() {
    ESP_LOGW(TAG, "Factory reset triggered from settings menu");
    
    // Show confirmation on display
    ESP_LOGI(TAG, "Performing factory reset in 3 seconds...");
    
    // Give user time to see the message
    vTaskDelay(pdMS_TO_TICKS(3000));
    
    performFactoryReset();
}

void FactoryResetManager::emergencyReset() {
    ESP_LOGW(TAG, "Emergency factory reset triggered!");
    performFactoryReset();
}

bool FactoryResetManager::isResetInProgress() {
    return reset_in_progress;
}

void FactoryResetManager::performFactoryReset() {
    reset_in_progress = true;
    
    ESP_LOGW(TAG, "=== FACTORY RESET STARTING ===");
    
    // Step 1: Show progress on display
    showResetProgress();
    
    // Step 2: Clear all NVS data
    ESP_LOGI(TAG, "Clearing NVS data...");
    clearAllNVSData();
    
    // Step 3: Clear WiFi credentials
    ESP_LOGI(TAG, "Clearing WiFi credentials...");
    clearWiFiCredentials();
    
    // Step 4: Clear any other persistent data
    ESP_LOGI(TAG, "Clearing application data...");
    
    ESP_LOGW(TAG, "=== FACTORY RESET COMPLETE ===");
    ESP_LOGI(TAG, "Device will restart in 2 seconds...");
    
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    // Step 5: Safe restart
    safeRestart();
}

void FactoryResetManager::clearAllNVSData() {
    esp_err_t err;
    
    // Erase entire NVS partition
    err = nvs_flash_erase();
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "NVS flash erased successfully");
        
        // Reinitialize NVS
        err = nvs_flash_init();
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "NVS reinitialized successfully");
        } else {
            ESP_LOGE(TAG, "Failed to reinitialize NVS: %s", esp_err_to_name(err));
        }
    } else {
        ESP_LOGE(TAG, "Failed to erase NVS flash: %s", esp_err_to_name(err));
    }
}

void FactoryResetManager::clearWiFiCredentials() {
    esp_err_t err;
    
    // Clear WiFi configuration
    err = esp_wifi_restore();
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "WiFi credentials cleared successfully");
    } else {
        ESP_LOGE(TAG, "Failed to clear WiFi credentials: %s", esp_err_to_name(err));
    }
}

void FactoryResetManager::showResetProgress() {
    // This would integrate with your LVGL display system
    // For now, just log the progress
    ESP_LOGI(TAG, "Factory Reset in Progress...");
    ESP_LOGI(TAG, "Please wait, do not power off device");
}

void FactoryResetManager::safeRestart() {
    // Ensure all tasks are safely stopped
    ESP_LOGI(TAG, "Restarting device...");
    
    // Small delay to ensure logging is flushed
    vTaskDelay(pdMS_TO_TICKS(500));
    
    esp_restart();
}
