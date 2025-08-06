#include "mqtt_manager.h"
#include <map>

static const char *TAG = "MQTT_MANAGER";

// Static member definitions
esp_mqtt_client_handle_t MQTTManager::mqtt_client = nullptr;
bool MQTTManager::mqtt_connected = false;
bool MQTTManager::last_mqtt_status = false;
bool MQTTManager::status_changed = false;

const char* MQTTManager::mqtt_server = "192.168.1.100";  // Default MQTT broker
const char* MQTTManager::mqtt_username = "";
const char* MQTTManager::mqtt_password = "";
const char* MQTTManager::mqtt_client_id = "ESP32-Knob";
int MQTTManager::mqtt_port = 1883;

// Organized MQTT Topics by Namespace
const char* MQTTManager::topics[] = {
    // Device control namespace (home/knob/*)
    "home/knob/command",        // Device control commands
    "home/knob/brightness",     // Screen brightness control
    "home/knob/haptic",         // Haptic feedback control
    
    // Energy monitoring namespace (emon/emontx3/*)
    "emon/emontx3/balance",     // Grid import/export balance (W)
    "emon/emontx3/solar",       // Solar generation (W)
    "emon/emontx3/vrms",        // Voltage RMS (V)
    "emon/emontx3/used",        // Energy consumption (W)
    "emon/emontx3/tariff",      // Tariff information
    
    // Future app namespaces (examples for scalability)
    // "weather/outdoor/temperature",  // Weather app
    // "weather/outdoor/humidity",
    // "lighting/living_room/brightness", // Lighting control
    // "audio/main/volume",            // Audio control
    // "hvac/main/temperature"         // HVAC control
};

const int MQTTManager::num_topics = sizeof(MQTTManager::topics) / sizeof(MQTTManager::topics[0]);

// Dynamic topic management for runtime additions
const int MQTTManager::max_dynamic_topics = 10;
String MQTTManager::dynamic_topics[10];
int MQTTManager::dynamic_topic_count = 0;

// Message handlers map
std::map<String, MQTTManager::MessageHandler> MQTTManager::handlers;

void MQTTManager::begin() {
    mqtt_connected = false;
    last_mqtt_status = false;
    status_changed = false;
    dynamic_topic_count = 0;
    
    // Generate unique client ID using MAC address
    uint8_t mac[6];
    WiFi.macAddress(mac);
    mqtt_client_id += String(mac[3], HEX) + String(mac[4], HEX) + String(mac[5], HEX);
    
    // Set MQTT callback
    mqtt.setCallback(defaultCallback);
    
    ESP_LOGI(TAG, "MQTT Manager initialized with client ID: %s", mqtt_client_id.c_str());
}

void MQTTManager::configure(const String& server, int port, const String& username, const String& password) {
    mqtt_server = server;
    mqtt_port = port;
    mqtt_username = username;
    mqtt_password = password;
    
    if (mqtt_server.length() > 0) {
        mqtt.setServer(mqtt_server.c_str(), mqtt_port);
        ESP_LOGI(TAG, "MQTT configured - Server: %s:%d, Username: %s", 
                 mqtt_server.c_str(), mqtt_port, 
                 mqtt_username.length() > 0 ? mqtt_username.c_str() : "(none)");
    }
}

bool MQTTManager::connect() {
    if (mqtt.connected()) {
        return true;
    }
    
    if (mqtt_server.length() == 0) {
        ESP_LOGW(TAG, "MQTT server not configured");
        return false;
    }
    
    ESP_LOGI(TAG, "Attempting MQTT connection to %s:%d...", mqtt_server.c_str(), mqtt_port);
    
    bool connected = false;
    if (mqtt_username.length() > 0 && mqtt_password.length() > 0) {
        connected = mqtt.connect(mqtt_client_id.c_str(), mqtt_username.c_str(), mqtt_password.c_str());
    } else {
        connected = mqtt.connect(mqtt_client_id.c_str());
    }
    
    if (connected) {
        ESP_LOGI(TAG, "MQTT connected successfully!");
        subscribeToTopics();
        mqtt_connected = true;
        status_changed = true;
        
        // Publish online status
        publish("home/knob/status", "online", true);
        
        return true;
    } else {
        ESP_LOGE(TAG, "MQTT connection failed, rc=%d", mqtt.state());
        mqtt_connected = false;
        return false;
    }
}

bool MQTTManager::isConnected() {
    return mqtt_connected;
}

bool MQTTManager::hasStatusChanged() {
    if (status_changed) {
        status_changed = false;
        return true;
    }
    return false;
}

void MQTTManager::process() {
    updateConnectionStatus();
    
    if (mqtt_connected) {
        mqtt.loop();
    }
}

void MQTTManager::subscribeToTopics() {
    ESP_LOGI(TAG, "Subscribing to %d static topics...", num_topics);
    
    // Subscribe to static topics
    for (int i = 0; i < num_topics; i++) {
        if (mqtt.subscribe(topics[i])) {
            ESP_LOGI(TAG, "✓ Subscribed to: %s", topics[i]);
        } else {
            ESP_LOGE(TAG, "✗ Failed to subscribe to: %s", topics[i]);
        }
    }
    
    // Subscribe to dynamic topics
    for (int i = 0; i < dynamic_topic_count; i++) {
        if (mqtt.subscribe(dynamic_topics[i].c_str())) {
            ESP_LOGI(TAG, "✓ Subscribed to dynamic topic: %s", dynamic_topics[i].c_str());
        } else {
            ESP_LOGE(TAG, "✗ Failed to subscribe to dynamic topic: %s", dynamic_topics[i].c_str());
        }
    }
}

void MQTTManager::registerHandler(const String& namespace_prefix, MessageHandler handler) {
    handlers[namespace_prefix] = handler;
    ESP_LOGI(TAG, "Registered handler for namespace: %s", namespace_prefix.c_str());
}

bool MQTTManager::publish(const String& topic, const String& message, bool retained) {
    if (!mqtt_connected) {
        ESP_LOGW(TAG, "Cannot publish - MQTT not connected");
        return false;
    }
    
    bool result = mqtt.publish(topic.c_str(), message.c_str(), retained);
    if (result) {
        ESP_LOGI(TAG, "Published to %s: %s", topic.c_str(), message.c_str());
    } else {
        ESP_LOGE(TAG, "Failed to publish to %s", topic.c_str());
    }
    
    return result;
}

bool MQTTManager::addTopic(const String& topic) {
    if (dynamic_topic_count >= max_dynamic_topics) {
        ESP_LOGE(TAG, "Cannot add topic - dynamic topic limit reached");
        return false;
    }
    
    dynamic_topics[dynamic_topic_count] = topic;
    dynamic_topic_count++;
    
    // If already connected, subscribe immediately
    if (mqtt_connected) {
        if (mqtt.subscribe(topic.c_str())) {
            ESP_LOGI(TAG, "✓ Added and subscribed to dynamic topic: %s", topic.c_str());
            return true;
        } else {
            ESP_LOGE(TAG, "✗ Failed to subscribe to dynamic topic: %s", topic.c_str());
            return false;
        }
    }
    
    ESP_LOGI(TAG, "Added dynamic topic (will subscribe on next connection): %s", topic.c_str());
    return true;
}

void MQTTManager::printTopics() {
    ESP_LOGI(TAG, "=== MQTT Topic List ===");
    ESP_LOGI(TAG, "Static topics (%d):", num_topics);
    for (int i = 0; i < num_topics; i++) {
        ESP_LOGI(TAG, "  %d: %s", i+1, topics[i]);
    }
    
    if (dynamic_topic_count > 0) {
        ESP_LOGI(TAG, "Dynamic topics (%d):", dynamic_topic_count);
        for (int i = 0; i < dynamic_topic_count; i++) {
            ESP_LOGI(TAG, "  %d: %s", i+1, dynamic_topics[i].c_str());
        }
    }
    ESP_LOGI(TAG, "=====================");
}

void MQTTManager::updateConnectionStatus() {
    bool current_status = mqtt.connected();
    
    if (current_status != last_mqtt_status) {
        mqtt_connected = current_status;
        last_mqtt_status = current_status;
        status_changed = true;
        
        if (mqtt_connected) {
            ESP_LOGI(TAG, "MQTT status: Connected");
        } else {
            ESP_LOGW(TAG, "MQTT status: Disconnected");
        }
    }
}

void MQTTManager::defaultCallback(char* topic, byte* payload, unsigned int length) {
    // Convert payload to string
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    String topic_str = String(topic);
    ESP_LOGI(TAG, "MQTT [%s]: %s", topic_str.c_str(), message.c_str());
    
    // Route message to appropriate handler
    routeMessage(topic_str, message);
}

void MQTTManager::routeMessage(const String& topic, const String& message) {
    String namespace_prefix = extractNamespace(topic);
    
    // Look for registered handler for this namespace
    auto handler_it = handlers.find(namespace_prefix);
    if (handler_it != handlers.end()) {
        ESP_LOGI(TAG, "Routing %s message to %s handler", topic.c_str(), namespace_prefix.c_str());
        handler_it->second(topic, message);
        return;
    }
    
    // Default built-in routing for core functionality
    if (namespace_prefix == "home/knob") {
        ESP_LOGI(TAG, "Processing device control message: %s = %s", topic.c_str(), message.c_str());
        
        if (topic == "home/knob/command") {
            ESP_LOGI(TAG, "Device command received: %s", message.c_str());
            // TODO: Process device commands
        } else if (topic == "home/knob/brightness") {
            ESP_LOGI(TAG, "Brightness command: %s", message.c_str());
            // TODO: Set display brightness
        } else if (topic == "home/knob/haptic") {
            ESP_LOGI(TAG, "Haptic command: %s", message.c_str());
            // TODO: Toggle haptic feedback
        }
    } else if (namespace_prefix == "emon/emontx3") {
        ESP_LOGI(TAG, "Processing energy monitoring message: %s = %s", topic.c_str(), message.c_str());
        // TODO: Route to energy data manager when implemented
        // EnergyDataManager::handleMessage(topic, message);
    } else {
        ESP_LOGW(TAG, "No handler found for namespace: %s (topic: %s)", namespace_prefix.c_str(), topic.c_str());
    }
}

String MQTTManager::extractNamespace(const String& topic) {
    // Extract namespace from topic (e.g., "emon/emontx3/balance" -> "emon/emontx3")
    int first_slash = topic.indexOf('/');
    if (first_slash == -1) {
        return topic; // No slash, return whole topic
    }
    
    int second_slash = topic.indexOf('/', first_slash + 1);
    if (second_slash == -1) {
        return topic.substring(0, first_slash); // Only one slash
    }
    
    return topic.substring(0, second_slash); // Return namespace prefix
}
