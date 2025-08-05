#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include <esp_log.h>
#include <functional>

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
    typedef std::function<void(const String& topic, const String& message)> MessageHandler;
    
    // Initialize MQTT management
    static void begin();
    
    // Set MQTT broker configuration
    static void configure(const String& server, int port, const String& username, const String& password);
    
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
    static void registerHandler(const String& namespace_prefix, MessageHandler handler);
    
    // Publish a message to a topic
    static bool publish(const String& topic, const String& message, bool retained = false);
    
    // Add a new topic to subscription list (for dynamic topics)
    static bool addTopic(const String& topic);
    
    // Get current topic list (for debugging)
    static void printTopics();

private:
    static WiFiClient espClient;
    static PubSubClient mqtt;
    static bool mqtt_connected;
    static bool last_mqtt_status;
    static bool status_changed;
    
    // MQTT Configuration
    static String mqtt_server;
    static String mqtt_username; 
    static String mqtt_password;
    static String mqtt_client_id;
    static int mqtt_port;
    
    // Topic management
    static const char* topics[];
    static const int num_topics;
    static const int max_dynamic_topics;
    static String dynamic_topics[];
    static int dynamic_topic_count;
    
    // Message handlers by namespace
    static std::map<String, MessageHandler> handlers;
    
    // Internal methods
    static void updateConnectionStatus();
    static void defaultCallback(char* topic, byte* payload, unsigned int length);
    static void routeMessage(const String& topic, const String& message);
    static String extractNamespace(const String& topic);
};
