#pragma once

/**
 * MQTT Integration Example
 * 
 * This file demonstrates how to integrate the new centralized MQTT architecture
 * with namespace-based topic organization into your ESP32-S3 Knob application.
 * 
 * Key Features:
 * ✅ Centralized MQTT connection management
 * ✅ Namespace-based topic organization (emon/*, home/*, etc.)
 * ✅ App-specific message handlers
 * ✅ Easy scaling for new applications
 * ✅ Dynamic topic subscription
 * ✅ Built-in message routing
 */

#include "core/mqtt_manager.h"
#include "features/energy/energy_data_manager.h"

/**
 * Initialize MQTT system with centralized architecture
 * Call this once during setup after WiFi is connected
 */
void mqtt_integration_init() {
    // Step 1: Initialize MQTT Manager
    MQTTManager::begin();
    
    // Step 2: Initialize app-specific managers
    EnergyDataManager::begin();
    
    // Step 3: Register additional custom handlers as needed
    // Example: Weather app handler
    // MQTTManager::registerHandler("weather", weatherAppHandler);
    
    // Example: Lighting control handler  
    // MQTTManager::registerHandler("lighting", lightingAppHandler);
}

/**
 * Configure MQTT connection with broker details
 * Call this with credentials from WiFiManager
 */
void mqtt_integration_configure(const String& server, int port, 
                               const String& username, const String& password) {
    MQTTManager::configure(server, port, username, password);
}

/**
 * Connect to MQTT broker
 * Call this after WiFi connection is established
 */
bool mqtt_integration_connect() {
    return MQTTManager::connect();
}

/**
 * Process MQTT messages
 * Call this in your main loop
 */
void mqtt_integration_loop() {
    MQTTManager::process();
}

/**
 * Check if MQTT status changed (for UI updates)
 */
bool mqtt_integration_status_changed() {
    return MQTTManager::hasStatusChanged();
}

/**
 * Check if connected
 */
bool mqtt_integration_is_connected() {
    return MQTTManager::isConnected();
}

/**
 * Example: Add a new topic dynamically
 * Useful for user-configurable topics or plugins
 */
bool mqtt_integration_add_topic(const String& topic) {
    return MQTTManager::addTopic(topic);
}

/**
 * Example: Publish device status
 */
void mqtt_integration_publish_status() {
    if (MQTTManager::isConnected()) {
        // Publish comprehensive device status
        String status = "{\"wifi\":true,\"mqtt\":true,\"ip\":\"" + WiFi.localIP().toString() + "\"}";
        MQTTManager::publish("home/knob/status", status, true);
        
        // Publish individual metrics
        MQTTManager::publish("home/knob/uptime", String(millis()/1000));
        MQTTManager::publish("home/knob/free_heap", String(ESP.getFreeHeap()));
    }
}

/**
 * Example custom handler for future weather app
 */
void weatherAppHandler(const String& topic, const String& message) {
    // Handle weather/* topics
    if (topic == "weather/outdoor/temperature") {
        float temp = message.toFloat();
        // Update weather display
        ESP_LOGI("WEATHER", "Outdoor temperature: %.1f°C", temp);
    } else if (topic == "weather/outdoor/humidity") {
        float humidity = message.toFloat();
        ESP_LOGI("WEATHER", "Outdoor humidity: %.1f%%", humidity);
    }
}

/**
 * Example custom handler for future lighting app  
 */
void lightingAppHandler(const String& topic, const String& message) {
    // Handle lighting/* topics
    if (topic == "lighting/living_room/brightness") {
        int brightness = message.toInt();
        ESP_LOGI("LIGHTING", "Living room brightness: %d%%", brightness);
        // Update lighting display or send commands
    }
}
