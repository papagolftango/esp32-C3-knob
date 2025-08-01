# MQTT Setup Guide for ESP32 Knob

This guide explains how to configure MQTT connectivity for your ESP32 Knob device using secure, non-hardcoded credentials.

## Overview

The ESP32 Knob uses WiFiManager's captive portal to securely collect and store MQTT broker credentials. This approach ensures:
- No hardcoded credentials in source code
- Secure credential storage in ESP32 flash memory
- Easy reconfiguration without code changes
- Professional deployment practices

## Initial Configuration

### 1. First Boot Setup

When the ESP32 Knob boots for the first time (or after a factory reset):

1. The device will create a WiFi Access Point named `ESP32-Knob-Setup`
2. Connect to this network from your phone/computer
3. A captive portal will automatically open (or navigate to 192.168.4.1)
4. Click "Configure WiFi"

### 2. WiFi Configuration

1. Select your WiFi network from the list
2. Enter your WiFi password
3. **Important**: Scroll down to see additional MQTT parameters

### 3. MQTT Broker Configuration

In the captive portal, you'll see these MQTT fields:

| Field | Description | Example |
|-------|-------------|---------|
| **MQTT Server** | Broker hostname or IP address | `broker.hivemq.com` or `192.168.1.100` |
| **MQTT Port** | Broker port (usually 1883 or 8883) | `1883` |
| **MQTT Username** | Your MQTT username | `myuser` |
| **MQTT Password** | Your MQTT password | `mypassword` |

4. Fill in all MQTT fields according to your broker setup
5. Click "Save" to store the configuration

## Supported MQTT Brokers

### Public Brokers (Testing Only)
- **HiveMQ**: `broker.hivemq.com:1883` (no auth required)
- **Eclipse**: `mqtt.eclipse.org:1883` (no auth required)
- **Mosquitto Test**: `test.mosquitto.org:1883` (no auth required)

### Self-Hosted Brokers
- **Mosquitto**: Most common self-hosted option
- **Home Assistant**: Built-in MQTT broker
- **Node-RED**: Can include MQTT broker functionality

### Cloud Brokers
- **AWS IoT Core**: Requires certificates (advanced setup)
- **Azure IoT Hub**: Requires connection strings
- **Google Cloud IoT**: Requires service account keys

## MQTT Topics

The ESP32 Knob subscribes to these topics by default:

| Topic | Purpose | Expected Values |
|-------|---------|----------------|
| `home/lighting/brightness` | Control lighting brightness | 0-100 (percentage) |
| `home/audio/volume` | Control audio volume | 0-100 (percentage) |
| `home/hvac/temperature` | Monitor/set temperature | Temperature value |
| `home/knob/command` | Send commands to knob | Custom commands |

### Publishing from Knob

The knob can publish to these topics:

| Topic | Purpose | Published Data |
|-------|---------|---------------|
| `home/knob/status` | Device status | `{"wifi":true,"mqtt":true,"ip":"192.168.1.100"}` |
| `home/knob/value` | Knob rotation value | 0-100 (percentage) |
| `home/knob/button` | Button press events | `{"pressed":true,"duration":500}` |

## Configuration Examples

### Example 1: Home Assistant MQTT

```
MQTT Server: homeassistant.local
MQTT Port: 1883
MQTT Username: mqtt_user
MQTT Password: your_secure_password
```

### Example 2: Local Mosquitto Broker

```
MQTT Server: 192.168.1.50
MQTT Port: 1883
MQTT Username: esp32_devices
MQTT Password: device_password_123
```

### Example 3: Cloud MQTT Service

```
MQTT Server: your-instance.cloudmqtt.com
MQTT Port: 1883
MQTT Username: provided_username
MQTT Password: provided_password
```

## Troubleshooting

### Connection Issues

#### WiFi Connected, MQTT Not Connected
1. Check MQTT broker is running and accessible
2. Verify server address and port are correct
3. Test credentials with MQTT client app (like MQTT Explorer)
4. Check firewall settings on broker

#### Both WiFi and MQTT Disconnected
1. Check WiFi network availability
2. Reset configuration: Hold button for 10+ seconds during boot
3. Reconfigure through captive portal

### Status Indicators

The knob's display shows connection status:

| Arc Color | Status |
|-----------|---------|
| 🟢 Green | WiFi + MQTT Connected |
| 🟠 Orange | WiFi Connected, MQTT Disconnected |
| 🔴 Red | WiFi Disconnected |

### Serial Monitor Debugging

Connect to the device via USB and monitor serial output at 115200 baud:

```
WiFi status changed: Connected
MQTT connecting to broker.hivemq.com:1883
MQTT connected successfully
MQTT status changed: Connected
MQTT subscribed to: home/lighting/brightness
MQTT subscribed to: home/audio/volume
MQTT subscribed to: home/hvac/temperature
MQTT subscribed to: home/knob/command
```

## Security Considerations

### Credential Storage
- Credentials are stored encrypted in ESP32 flash memory
- Use strong, unique passwords for MQTT accounts
- Consider separate MQTT user for IoT devices

### Network Security
- Use WPA2/WPA3 for WiFi networks
- Consider MQTT over SSL/TLS (port 8883) for production
- Isolate IoT devices on separate network segment

### Access Control
- Create dedicated MQTT user for ESP32 devices
- Limit topic access permissions in broker configuration
- Regularly rotate MQTT passwords

## Factory Reset

To clear all stored configuration and start fresh:

1. Power on the device
2. Within 10 seconds of boot, press and hold the capacitive touch area
3. Hold for 10+ seconds until you see "Factory Reset" message
4. Release and the device will restart in setup mode

## Integration Examples

### Home Assistant Configuration

Add to your `configuration.yaml`:

```yaml
sensor:
  - platform: mqtt
    name: "ESP32 Knob Value"
    state_topic: "home/knob/value"
    unit_of_measurement: "%"

automation:
  - alias: "Knob Controls Brightness"
    trigger:
      platform: mqtt
      topic: "home/knob/value"
    action:
      service: light.turn_on
      target:
        entity_id: light.living_room
      data:
        brightness_pct: "{{ trigger.payload }}"
```

### Node-RED Flow

Create a flow that:
1. Subscribes to `home/knob/value`
2. Maps knob value to desired ranges
3. Publishes to device-specific topics

## Next Steps

1. **Test the configuration** with a public broker first
2. **Set up your own broker** for production use
3. **Customize MQTT topics** for your smart home setup
4. **Implement security** with proper authentication
5. **Create automations** in your home automation system

For advanced topics like SSL/TLS encryption and certificate management, refer to your MQTT broker's documentation.
