#pragma once

#include <esp_wifi.h>
#include <mqtt_client.h>
#include <esp_log.h>
#include <functional>
#include <map>

/**
 * Centralized MQTT Manager with Namespace-based Topic Organization
 * 
 * This manager provides:
 * - Single MQTT connection for efficient resource usage
 * - Centralized subscription management
 * - Domain-based namespacing (emon/, home/, weather/, etc.)
 * - App-specific message handlers
 * - Easy scaling for new applications
 */

class MQTTManager {
public:
    // Message handler function type
    typedef std::function<void(const char* topic, const char* message)> MessageHandler;
    
    // Initialize MQTT management
    static void begin();
    
    // Set MQTT broker configuration
    static void configure(const char* server, int port, const char* username, const char* password);
    
    // Connect to MQTT broker
    static bool connect();
    
    // Check if MQTT is connected
    static bool isConnected();
    
    // Get connection status change (for UI updates)
    static bool hasStatusChanged();
    
    // Process MQTT messages (call in loop)
    static void process();
    
    // Subscribe to all configured topics
    static void subscribeToTopics();
    
    // Register a message handler for a topic namespace
    static void registerHandler(const char* namespace_prefix, MessageHandler handler);
    
    // Publish a message to a topic
    static bool publish(const char* topic, const char* message, bool retained = false);
    
    // Add a new topic to subscription list (for dynamic topics)
    static bool addTopic(const char* topic);
    
    // Get current topic list (for debugging)
    static void printTopics();

private:
    static esp_mqtt_client_handle_t mqtt_client;
    static bool mqtt_connected;
    static bool last_mqtt_status;
    static bool status_changed;
    
    // MQTT Configuration
    static const char* mqtt_server;
    static const char* mqtt_username; 
    static const char* mqtt_password;
    static const char* mqtt_client_id;
    static int mqtt_port;
    
    // Topic management
    static const char* topics[];
    static const int num_topics;
    static const int max_dynamic_topics;
    static char dynamic_topics[][64];  // Fixed-size strings for ESP-IDF
    static int dynamic_topic_count;
    
    // Message handlers by namespace
    static std::map<const char*, MessageHandler> handlers;
    
    // Internal methods
    static void updateConnectionStatus();
    static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
    static void routeMessage(const char* topic, const char* message);
    static const char* extractNamespace(const char* topic);
};
