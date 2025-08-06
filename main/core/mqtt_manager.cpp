#include "mqtt_manager.h"
#include <cstring>

static const char *TAG = "MQTT_MANAGER";

// Static member definitions
esp_mqtt_client_handle_t MQTTManager::mqtt_client = nullptr;
bool MQTTManager::mqtt_connected = false;
bool MQTTManager::last_mqtt_status = false;
bool MQTTManager::status_changed = false;

const char* MQTTManager::mqtt_server = "192.168.1.100";
const char* MQTTManager::mqtt_username = "";
const char* MQTTManager::mqtt_password = "";
const char* MQTTManager::mqtt_client_id = "ESP32-Knob";
int MQTTManager::mqtt_port = 1883;

// Organized MQTT Topics by Namespace
const char* MQTTManager::topics[] = {
    // Device control namespace (home/knob/*)
    "home/knob/command",        
    "home/knob/brightness",     
    "home/knob/haptic",         
    
    // Energy monitoring namespace (emon/emontx3/*)
    "emon/emontx3/balance",     
    "emon/emontx3/solar",       
    "emon/emontx3/import",        
    "emon/emontx3/used",        
    "emon/emontx3/tariff",
    
    // House information namespace (home/*)
    "home/motd"
};

const int MQTTManager::num_topics = sizeof(MQTTManager::topics) / sizeof(MQTTManager::topics[0]);
const int MQTTManager::max_dynamic_topics = 10;
char MQTTManager::dynamic_topics[10][64] = {0};
int MQTTManager::dynamic_topic_count = 0;

std::map<const char*, MQTTManager::MessageHandler> MQTTManager::handlers;

void MQTTManager::begin() {
    ESP_LOGI(TAG, "Initializing MQTT Manager");
    mqtt_connected = false;
    last_mqtt_status = false;
    status_changed = false;
}

void MQTTManager::configure(const char* server, int port, const char* username, const char* password) {
    mqtt_server = server;
    mqtt_port = port;
    mqtt_username = username;
    mqtt_password = password;
    ESP_LOGI(TAG, "MQTT configured: %s:%d", mqtt_server, mqtt_port);
}

bool MQTTManager::connect() {
    if (mqtt_client != nullptr) {
        ESP_LOGW(TAG, "MQTT client already exists");
        return mqtt_connected;
    }

    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = mqtt_server;
    mqtt_cfg.broker.address.port = mqtt_port;
    mqtt_cfg.credentials.username = mqtt_username;
    mqtt_cfg.credentials.authentication.password = mqtt_password;
    mqtt_cfg.credentials.client_id = mqtt_client_id;

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == nullptr) {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return false;
    }

    esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, mqtt_event_handler, nullptr);
    
    esp_err_t result = esp_mqtt_client_start(mqtt_client);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start MQTT client: %s", esp_err_to_name(result));
        return false;
    }

    ESP_LOGI(TAG, "MQTT client started");
    return true;
}

bool MQTTManager::isConnected() {
    return mqtt_connected;
}

bool MQTTManager::hasStatusChanged() {
    bool changed = status_changed;
    status_changed = false;  // Reset flag after reading
    return changed;
}

void MQTTManager::process() {
    // ESP-IDF MQTT client handles processing in background tasks
    // We just need to update status tracking
    updateConnectionStatus();
}

void MQTTManager::subscribeToTopics() {
    if (!mqtt_connected || mqtt_client == nullptr) {
        ESP_LOGW(TAG, "Cannot subscribe - MQTT not connected");
        return;
    }

    // Subscribe to all predefined topics
    for (int i = 0; i < num_topics; i++) {
        int msg_id = esp_mqtt_client_subscribe(mqtt_client, topics[i], 1);
        ESP_LOGI(TAG, "Subscribed to %s, msg_id=%d", topics[i], msg_id);
    }

    // Subscribe to dynamic topics
    for (int i = 0; i < dynamic_topic_count; i++) {
        int msg_id = esp_mqtt_client_subscribe(mqtt_client, dynamic_topics[i], 1);
        ESP_LOGI(TAG, "Subscribed to dynamic topic %s, msg_id=%d", dynamic_topics[i], msg_id);
    }
}

void MQTTManager::registerHandler(const char* namespace_prefix, MessageHandler handler) {
    handlers[namespace_prefix] = handler;
    ESP_LOGI(TAG, "Registered handler for namespace: %s", namespace_prefix);
}

bool MQTTManager::publish(const char* topic, const char* message, bool retained) {
    if (!mqtt_connected || mqtt_client == nullptr) {
        ESP_LOGW(TAG, "Cannot publish - MQTT not connected");
        return false;
    }

    int msg_id = esp_mqtt_client_publish(mqtt_client, topic, message, 0, 1, retained ? 1 : 0);
    if (msg_id >= 0) {
        ESP_LOGI(TAG, "Published to %s: %s (msg_id=%d)", topic, message, msg_id);
        return true;
    } else {
        ESP_LOGE(TAG, "Failed to publish to %s", topic);
        return false;
    }
}

bool MQTTManager::addTopic(const char* topic) {
    if (dynamic_topic_count >= max_dynamic_topics) {
        ESP_LOGW(TAG, "Cannot add topic - dynamic topic list full");
        return false;
    }

    strncpy(dynamic_topics[dynamic_topic_count], topic, 63);
    dynamic_topics[dynamic_topic_count][63] = '\0';  // Ensure null termination
    dynamic_topic_count++;
    
    ESP_LOGI(TAG, "Added dynamic topic: %s", topic);
    return true;
}

void MQTTManager::printTopics() {
    ESP_LOGI(TAG, "=== MQTT Topics ===");
    ESP_LOGI(TAG, "Predefined topics (%d):", num_topics);
    for (int i = 0; i < num_topics; i++) {
        ESP_LOGI(TAG, "  %s", topics[i]);
    }
    
    ESP_LOGI(TAG, "Dynamic topics (%d):", dynamic_topic_count);
    for (int i = 0; i < dynamic_topic_count; i++) {
        ESP_LOGI(TAG, "  %s", dynamic_topics[i]);
    }
}

void MQTTManager::updateConnectionStatus() {
    bool current_status = mqtt_connected;
    if (current_status != last_mqtt_status) {
        status_changed = true;
        last_mqtt_status = current_status;
    }
}

void MQTTManager::mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            mqtt_connected = true;
            subscribeToTopics();
            break;
            
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            mqtt_connected = false;
            break;
            
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
            
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
            
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
            
        case MQTT_EVENT_DATA: {
            // Null-terminate the topic and data for safe string operations
            char topic[256] = {0};
            char data[512] = {0};
            
            if (event->topic_len < sizeof(topic)) {
                memcpy(topic, event->topic, event->topic_len);
            }
            if (event->data_len < sizeof(data)) {
                memcpy(data, event->data, event->data_len);
            }
            
            ESP_LOGI(TAG, "MQTT_EVENT_DATA: %s = %s", topic, data);
            routeMessage(topic, data);
            break;
        }
            
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
            
        default:
            ESP_LOGI(TAG, "Other event id:%d", event_id);
            break;
    }
}

void MQTTManager::routeMessage(const char* topic, const char* message) {
    const char* namespace_prefix = extractNamespace(topic);
    
    if (namespace_prefix && handlers.find(namespace_prefix) != handlers.end()) {
        handlers[namespace_prefix](topic, message);
    } else {
        ESP_LOGW(TAG, "No handler for namespace: %s (topic: %s)", 
                 namespace_prefix ? namespace_prefix : "unknown", topic);
    }
}

const char* MQTTManager::extractNamespace(const char* topic) {
    // Simple namespace extraction - return everything up to second '/'
    static char namespace_buffer[64];
    
    if (!topic) return nullptr;
    
    const char* first_slash = strchr(topic, '/');
    if (!first_slash) return nullptr;
    
    const char* second_slash = strchr(first_slash + 1, '/');
    if (!second_slash) {
        // No second slash, use entire topic as namespace
        strncpy(namespace_buffer, topic, sizeof(namespace_buffer) - 1);
        namespace_buffer[sizeof(namespace_buffer) - 1] = '\0';
        return namespace_buffer;
    }
    
    // Copy up to second slash
    size_t len = second_slash - topic;
    if (len >= sizeof(namespace_buffer)) len = sizeof(namespace_buffer) - 1;
    
    memcpy(namespace_buffer, topic, len);
    namespace_buffer[len] = '\0';
    
    return namespace_buffer;
}
