# Centralized MQTT Architecture Implementation

## Overview

Your ESP32-S3 Knob now implements a **hybrid centralized MQTT architecture** with namespace-based topic organization. This provides efficient resource usage while maintaining clear separation of concerns across different applications.

## 🏗️ Architecture Components

### 1. **MQTTManager** (`core/mqtt_manager.h/.cpp`)
- **Single MQTT connection** for the entire device
- **Centralized subscription management** with both static and dynamic topics
- **Namespace-based message routing** to app-specific handlers
- **Built-in connection management** and status monitoring

### 2. **EnergyDataManager** (`features/energy/energy_data_manager.h/.cpp`)
- **Example app-specific handler** for EmonTX3 energy monitoring
- **Data validation** and **status tracking**
- **Clean API** for accessing energy data from UI components

### 3. **Integration Layer** (`mqtt_integration_example.h`)
- **Simple setup functions** for easy integration
- **Example handlers** for future apps (weather, lighting, etc.)
- **Status publishing** and **dynamic topic management**

## 📋 Topic Organization

### Current Namespaces

| Namespace | Purpose | Example Topics |
|-----------|---------|----------------|
| `home/knob/*` | Device control | `home/knob/command`, `home/knob/brightness` |
| `emon/emontx3/*` | Energy monitoring | `emon/emontx3/balance`, `emon/emontx3/solar` |

### Future Scalability

```cpp
// Easy to add new app domains:
"weather/outdoor/temperature"     // Weather app
"lighting/living_room/brightness" // Lighting control  
"audio/main/volume"              // Audio control
"hvac/main/temperature"          // HVAC control
```

## 🚀 Integration Steps

### 1. **Replace Old MQTT Code**
Remove any existing MQTT code from your `main.cpp` and replace with:

```cpp
#include "mqtt_integration_example.h"

void setup() {
    // ... existing setup code ...
    
    // Initialize MQTT system
    mqtt_integration_init();
    
    // Configure with WiFiManager credentials
    mqtt_integration_configure(mqtt_server, mqtt_port, mqtt_username, mqtt_password);
}

void loop() {
    // Process MQTT messages
    mqtt_integration_loop();
    
    // Check for energy data updates
    if (EnergyDataManager::hasDataChanged()) {
        // Update energy display
        update_energy_display();
    }
    
    // Check for MQTT status changes
    if (mqtt_integration_status_changed()) {
        // Update connection status in UI
        update_mqtt_status_display();
    }
}
```

### 2. **Connect After WiFi**
In your WiFi connection callback:

```cpp
void wifi_connected_callback() {
    // Try to connect MQTT after WiFi connects
    if (mqtt_integration_connect()) {
        ESP_LOGI("MAIN", "MQTT connected successfully");
        mqtt_integration_publish_status();
    }
}
```

### 3. **Access Energy Data**
From your UI components:

```cpp
void update_energy_display() {
    float balance = EnergyDataManager::getBalance();
    float solar = EnergyDataManager::getSolar();
    float used = EnergyDataManager::getUsed();
    
    // Update your mini-arcs or displays
    update_balance_arc(balance);
    update_solar_arc(solar);
    update_usage_arc(used);
}
```

## 🔧 Adding New Applications

### Step 1: Create App Handler
```cpp
void weatherAppHandler(const String& topic, const String& message) {
    if (topic == "weather/outdoor/temperature") {
        float temp = message.toFloat();
        // Process temperature data
    }
}
```

### Step 2: Register Handler
```cpp
void setup() {
    MQTTManager::registerHandler("weather", weatherAppHandler);
}
```

### Step 3: Add Topics (Optional)
```cpp
// Add dynamic topics at runtime
MQTTManager::addTopic("weather/outdoor/temperature");
MQTTManager::addTopic("weather/outdoor/humidity");
```

## 📊 Benefits Achieved

✅ **Single MQTT Connection** - Efficient ESP32 resource usage  
✅ **Namespace Organization** - Clear topic structure (`emon/`, `home/`, `weather/`)  
✅ **App Separation** - Each app has its own handler and data management  
✅ **Easy Scaling** - Add new apps by registering handlers  
✅ **Central Management** - One place to debug MQTT issues  
✅ **Dynamic Topics** - Add topics at runtime for user configuration  
✅ **Data Validation** - Built-in validation for energy data  
✅ **Status Monitoring** - Connection status and data freshness tracking  

## 🐛 Debugging Features

### View All Topics
```cpp
MQTTManager::printTopics(); // Logs all subscribed topics
```

### Monitor Message Flow
All MQTT messages are logged with:
- Topic name
- Message content  
- Routing decisions
- Handler assignments

### Check Data Validity
```cpp
if (!EnergyDataManager::isDataValid()) {
    ESP_LOGW("MAIN", "Energy data is stale - check MQTT connection");
}
```

## 🔄 Migration from Old Code

The new architecture maintains **backward compatibility** with your existing topic structure while adding the benefits of centralized management. Your EmonTX3 integration will work immediately with improved error handling and data validation.

## 🎯 Next Steps

1. **Replace old MQTT code** with the new centralized system
2. **Test with your existing EmonTX3 topics** 
3. **Add new app handlers** as needed (weather, lighting, etc.)
4. **Configure dynamic topics** for user-customizable MQTT integration

This architecture scales from your current energy monitoring to a full smart home control hub while maintaining efficient resource usage on the ESP32.
