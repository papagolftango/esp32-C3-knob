#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include "../../features/energy/energy_data.h"

class MQTTManager {
public:
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
    
    // Subscribe to topics
    static void subscribeToTopics();
    
    // Set message callback
    static void setCallback(void (*callback)(char*, byte*, unsigned int));
    
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
    
    // Topics to subscribe to
    static const char* topics[];
    static const int num_topics;
    
    static void updateConnectionStatus();
    static void defaultCallback(char* topic, byte* payload, unsigned int length);
};
