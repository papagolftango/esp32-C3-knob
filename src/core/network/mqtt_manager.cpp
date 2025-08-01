#include "mqtt_manager.h"

// Static member definitions
WiFiClient MQTTManager::espClient;
PubSubClient MQTTManager::mqtt(espClient);
bool MQTTManager::mqtt_connected = false;
bool MQTTManager::last_mqtt_status = false;
bool MQTTManager::status_changed = false;

String MQTTManager::mqtt_server = "192.168.1.100";
String MQTTManager::mqtt_username = "";
String MQTTManager::mqtt_password = "";
String MQTTManager::mqtt_client_id = "ESP32-Knob-";
int MQTTManager::mqtt_port = 1883;

// MQTT Topics to subscribe to (EmonTX3 + control topics)
const char* MQTTManager::topics[] = {
    "home/knob/command",        // Device control
    "emon/emontx3/balance",     // Grid balance (import/export)
    "emon/emontx3/solar",       // Solar generation
    "emon/emontx3/vrms",        // Voltage RMS
    "emon/emontx3/used",        // House usage
    "emon/emontx3/tariff"       // Tariff information
};

const int MQTTManager::num_topics = sizeof(MQTTManager::topics) / sizeof(MQTTManager::topics[0]);

void MQTTManager::begin() {
    mqtt_connected = false;
    last_mqtt_status = false;
    status_changed = false;
    
    // Generate unique client ID
    mqtt_client_id += String((uint32_t)ESP.getEfuseMac(), HEX);
    
    // Set default callback
    mqtt.setCallback(defaultCallback);
}

void MQTTManager::configure(const String& server, int port, const String& username, const String& password) {
    mqtt_server = server;
    mqtt_port = port;
    mqtt_username = username;
    mqtt_password = password;
    
    mqtt.setServer(mqtt_server.c_str(), mqtt_port);
}

bool MQTTManager::connect() {
    if (mqtt.connected()) {
        return true;
    }
    
    Serial.printf("Attempting MQTT connection to %s:%d...", mqtt_server.c_str(), mqtt_port);
    
    bool connected = false;
    if (mqtt_username.length() > 0) {
        connected = mqtt.connect(mqtt_client_id.c_str(), mqtt_username.c_str(), mqtt_password.c_str());
    } else {
        connected = mqtt.connect(mqtt_client_id.c_str());
    }
    
    if (connected) {
        Serial.println(" connected!");
        subscribeToTopics();
        mqtt_connected = true;
        status_changed = true;
        return true;
    } else {
        Serial.printf(" failed, rc=%d\n", mqtt.state());
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
    for (int i = 0; i < num_topics; i++) {
        mqtt.subscribe(topics[i]);
        Serial.printf("Subscribed to: %s\n", topics[i]);
    }
}

void MQTTManager::setCallback(void (*callback)(char*, byte*, unsigned int)) {
    mqtt.setCallback(callback);
}

void MQTTManager::updateConnectionStatus() {
    bool current_status = mqtt.connected();
    
    if (current_status != last_mqtt_status) {
        mqtt_connected = current_status;
        last_mqtt_status = current_status;
        status_changed = true;
        
        Serial.printf("MQTT status changed: %s\n", mqtt_connected ? "Connected" : "Disconnected");
    }
}

void MQTTManager::defaultCallback(char* topic, byte* payload, unsigned int length) {
    // Convert payload to string
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.printf("MQTT [%s]: %s\n", topic, message.c_str());
    
    // Route messages to appropriate handlers
    String topic_str = String(topic);
    
    if (topic_str == "emon/emontx3/balance") {
        EnergyData_Manager::updateBalance(message.toFloat());
    } else if (topic_str == "emon/emontx3/solar") {
        EnergyData_Manager::updateSolar(message.toFloat());
    } else if (topic_str == "emon/emontx3/vrms") {
        EnergyData_Manager::updateVrms(message.toFloat());
    } else if (topic_str == "emon/emontx3/used") {
        EnergyData_Manager::updateUsed(message.toFloat());
    } else if (topic_str == "emon/emontx3/tariff") {
        EnergyData_Manager::updateTariff(message);
    } else if (topic_str == "home/knob/command") {
        // Handle device commands here
        Serial.printf("Device command received: %s\n", message.c_str());
    }
}
