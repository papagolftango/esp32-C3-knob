# Home/Knob MQTT Topics Usage Guide

## 📡 Overview

The `home/knob/*` namespace handles **device control and status** for your ESP32-S3 Knob. These topics allow external systems (Home Assistant, Node-RED, mobile apps, etc.) to control your device and receive status updates.

## 🏠 Topic Structure

### **Incoming Control Topics** (Subscribe)
Your ESP32 knob listens for these commands:

| Topic | Purpose | Valid Values | Example |
|-------|---------|--------------|---------|
| `home/knob/command` | General device commands | `reboot`, `factory_reset`, `wifi_reset`, `status` | `reboot` |
| `home/knob/brightness` | Screen brightness control | `0-100` (percentage) | `75` |
| `home/knob/haptic` | Haptic feedback control | `on`, `off`, `toggle`, `true`, `false` | `on` |

### **Outgoing Status Topics** (Publish)
Your ESP32 knob publishes these status updates:

| Topic | Purpose | Data Format | Example |
|-------|---------|-------------|---------|
| `home/knob/status` | Complete device status | JSON object | `{"wifi":true,"mqtt":true,"ip":"192.168.1.100","brightness":75,"haptic":true}` |
| `home/knob/brightness/status` | Current brightness level | `0-100` | `75` |
| `home/knob/haptic/status` | Current haptic state | `on`/`off` | `on` |

## 🎮 Control Examples

### **From Command Line (Mosquitto)**
```bash
# Set brightness to 50%
mosquitto_pub -h your-broker -t "home/knob/brightness" -m "50"

# Enable haptic feedback
mosquitto_pub -h your-broker -t "home/knob/haptic" -m "on"

# Reboot the device
mosquitto_pub -h your-broker -t "home/knob/command" -m "reboot"

# Request status update
mosquitto_pub -h your-broker -t "home/knob/command" -m "status"
```

### **From Home Assistant**
```yaml
# automation.yaml
automation:
  - alias: "Dim knob at night"
    trigger:
      platform: time
      at: "22:00:00"
    action:
      service: mqtt.publish
      data:
        topic: "home/knob/brightness"
        payload: "25"

  - alias: "Brighten knob in morning"  
    trigger:
      platform: time
      at: "07:00:00"
    action:
      service: mqtt.publish
      data:
        topic: "home/knob/brightness"
        payload: "100"
```

### **From Node-RED**
```javascript
// Set brightness based on time of day
var hour = new Date().getHours();
var brightness = 100;

if (hour >= 22 || hour <= 6) {
    brightness = 25;  // Night mode
} else if (hour >= 19) {
    brightness = 50;  // Evening
}

msg.payload = brightness.toString();
msg.topic = "home/knob/brightness";
return msg;
```

## ⚙️ Settings Screen Integration

Your settings screen now **integrates with MQTT control**:

### **Physical Settings Screen:**
- **Rotate:** Navigate between WiFi Reset, Display Brightness, Haptic Feedback, Factory Reset
- **Press:** Execute the selected action

### **MQTT Remote Control:**
- **Same functions** can be triggered via MQTT topics
- **Status changes** are published to MQTT automatically
- **Synchronization** between physical and remote control

### **Brightness Control Flow:**
1. **Settings Screen:** Press on "Display Brightness" → cycles 25%→50%→75%→100%→25%
2. **MQTT Control:** `mosquitto_pub -t "home/knob/brightness" -m "75"` → sets to 75%
3. **Status Publishing:** Device publishes `home/knob/brightness/status` with new value

### **Haptic Control Flow:**
1. **Settings Screen:** Press on "Haptic Feedback" → toggles on/off
2. **MQTT Control:** `mosquitto_pub -t "home/knob/haptic" -m "toggle"` → toggles state
3. **Status Publishing:** Device publishes `home/knob/haptic/status` with new state

## 🔄 Data Flow Architecture

```
External System        MQTT Broker         ESP32 Knob         Settings Screen
     │                      │                   │                   │
     ├─ brightness: 75 ────→│                   │                   │
     │                      ├─────────────────→│                   │
     │                      │                   ├─ setBrightness() │
     │                      │                   │                   │
     │                      │                   ├─ publishStatus() │
     │                      │←─────────────────│                   │
     │                      │                   │                   │
     │                      │                   │←─ button press ──│
     │                      │                   ├─ cycleBrightness()│
     │                      │                   ├─ publishStatus() │
     │                      │←─────────────────│                   │
```

## 🏗️ Implementation Details

### **DeviceControlManager Class:**
- **Handles all `home/knob/*` topics**
- **Integrates with settings screen functions**  
- **Publishes status updates automatically**
- **Validates incoming commands**

### **Settings Screen Integration:**
- **`settings_screen_cycle_brightness()`** - Physical brightness control
- **`settings_screen_toggle_haptic()`** - Physical haptic control
- **Both trigger MQTT status publishing**

### **Synchronization:**
- **Physical actions** publish MQTT status updates
- **MQTT commands** update physical settings
- **Always in sync** between local and remote control

## 🚀 Use Cases

### **Home Automation:**
- **Automatically dim** display at night
- **Disable haptic** when sleeping hours
- **Reboot device** during maintenance windows

### **Mobile App Control:**
- **Brightness slider** sends MQTT commands
- **Settings toggles** control device remotely
- **Status monitoring** via MQTT subscriptions

### **Voice Control:**
- **"Set knob brightness to 50%"** → MQTT command
- **"Turn off knob haptic"** → MQTT command
- **"Reboot knob device"** → MQTT command

### **Smart Scenes:**
- **"Movie Mode"** → Dim display, disable haptic
- **"Work Mode"** → Full brightness, enable haptic
- **"Sleep Mode"** → Minimum brightness, disable haptic

This integration gives you **full remote control** of your ESP32 knob while maintaining **local physical control** through the settings screen!
