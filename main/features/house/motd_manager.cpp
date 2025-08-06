#include "motd_manager.h"
#include "esp_log.h"

static const char* TAG = "motd_manager";

// Static member definitions
std::string MOTDManager::current_message = "";
bool MOTDManager::message_changed = false;
bool MOTDManager::initialized = false;

void MOTDManager::init() {
    if (initialized) return;
    
    current_message = getDefaultMessage();
    message_changed = false;
    initialized = true;
    
    ESP_LOGI(TAG, "MOTD Manager initialized");
}

void MOTDManager::updateMessage(const char* message) {
    if (!message) return;
    
    std::string new_message(message);
    
    // Only update if the message has actually changed
    if (new_message != current_message) {
        current_message = new_message;
        message_changed = true;
        ESP_LOGI(TAG, "MOTD updated: %s", message);
    }
}

std::string MOTDManager::getCurrentMessage() {
    if (!initialized) {
        init();
    }
    
    return current_message.empty() ? getDefaultMessage() : current_message;
}

bool MOTDManager::hasMessageChanged() {
    return message_changed;
}

void MOTDManager::clearChangeFlag() {
    message_changed = false;
}

std::string MOTDManager::getDefaultMessage() {
    return "Welcome to Smart Home • Bin Collection Status • Energy Monitoring Active";
}
