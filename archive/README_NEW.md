# ESP32-S3 Energy Monitoring Knob 🎯⚡

A sophisticated energy monitoring interface using **ESP32-S3-Knob-Touch-LCD-1.8** with LVGL graphics, EmonTX3 MQTT integration, and daily peak tracking.

![Hardware](https://img.shields.io/badge/Hardware-ESP32--S3--Knob-blue)
![Display](https://img.shields.io/badge/Display-360x360-green)
![Status](https://img.shields.io/badge/Status-Ready%20to%20Deploy-brightgreen)

## ✨ Features

### 🎨 **Multi-Screen Interface**
- **Energy Monitor**: Real-time solar/grid/usage with balance arc + peak tracking
- **Weather Display**: Local weather conditions and UV index
- **House Info**: Daily energy summaries and statistics  
- **Settings**: Configuration and demo mode controls

### ⚡ **Advanced Energy Monitoring**
- **Balance Arc**: Large central arc showing import/export with color coding
- **Mini Arcs**: Solar generation (left) and house usage (right)
- **Daily Peak Tracking**: Visual dots showing peak import/export on arc
- **Tariff Indicators**: Color-coded rate indicators (standard/peak)
- **EmonTX3 Integration**: Real-time MQTT data from energy monitoring

### 🎯 **Built-in Demo Mode**
- **Mock Data Generator**: Realistic 24-hour energy cycle in 2 minutes
- **No Hardware Required**: Test interface without EmonTX3
- **Visual Development**: Perfect for UI testing and client demos

### 🔧 **Professional Features**
- **Secure WiFi Setup**: WiFiManager with captive portal
- **Rotary Navigation**: Hardware encoder for screen switching
- **Touch Interface**: Touchscreen for interactions
- **MQTT Ready**: Secure broker connection with authentication
- **Peak Reset**: Daily reset at configurable time (default: noon)

## 🛠️ Hardware

### ESP32-S3-Knob-Touch-LCD-1.8
- **Dual MCU System**: ESP32-S3R8 + ESP32-U4WDH
- **Display**: 360×360 ST77916 TFT with touch
- **Encoder**: Rotation-only (GPIO 7/8)
- **Memory**: 8MB PSRAM, 4MB Flash

### Pin Configuration
```cpp
// Display SPI
#define TFT_MOSI     2
#define TFT_SCLK     3
#define TFT_CS       5
#define TFT_DC       6
#define TFT_RST      1
#define TFT_BL       10

// Touch & Encoder
#define TOUCH_CS     0
#define ENCODER_A    8
#define ENCODER_B    7
```

## 🚀 Quick Start

### 1. **Hardware Setup**
```bash
# Clone repository
git clone https://github.com/papagolftango/ESP32-C3-Knob.git
cd ESP32-C3-Knob

# Install PlatformIO extension in VS Code
# Open project folder in VS Code
```

### 2. **Demo Mode (No EmonTX3 Required)**
```cpp
// In src/main.cpp - Enable demo mode
#define ENABLE_MOCK_DATA true
```
```bash
# Build and upload
pio run -t upload -t monitor

# Watch simulated energy data cycle through 24 hours
```

### 3. **Production Setup**
```cpp
// Disable demo mode for real data
#define ENABLE_MOCK_DATA false
```

**Configure via WiFiManager:**
- Power on device
- Connect to "ESP32-Knob-XXXXXX" WiFi
- Set MQTT broker: `your-broker.com:1883`
- Enter EmonTX3 credentials

## 📊 Energy Display

### Balance Arc (Center)
```
     ╭─────────────╮
   ╱     EXPORT     ╲     🟢 Good export (>1.5kW)
  │      1200W      │     🟡 Low export (≤1.5kW)  
   ╲       ●       ╱      🔴 Grid import
     ╲_____●_____╱       ⚪ Balanced

Peak dots: 🟢 Daily export peak | 🔴 Daily import peak
```

### Mini Arcs
- **Left (Solar)**: ☀️ 0-5kW generation scale
- **Right (Usage)**: 🏠 0-8kW consumption scale

### MQTT Topics (EmonTX3)
```
emon/emontx3/balance  # Grid import/export balance
emon/emontx3/solar    # Solar generation
emon/emontx3/used     # House consumption  
emon/emontx3/vrms     # Voltage RMS
emon/emontx3/tariff   # Rate indicator
```

## 🎬 Demo & Development

### Built-in Simulator
The project includes comprehensive simulation capabilities:

- **Visual Mockup**: ASCII art preview in `simulator/visual_mockup.py`
- **LVGL Simulator**: PC-based preview in `simulator/energy_screen_simulator.c`
- **Mock Data**: Realistic energy patterns with peak tracking

### Daily Cycle Simulation
```
Morning (6-10h):   High usage + building solar → Import
Midday (10-16h):   Peak solar + low usage → Export  
Evening (16-22h):  High usage + declining solar → Import
Night (22-6h):     Base load only → Minimal import
```

## 📁 Project Structure

```
ESPKnob/
├── src/main.cpp                 # Main application code
├── platformio.ini               # Build configuration  
├── simulator/                   # Visual development tools
│   ├── visual_mockup.py         # ASCII art generator
│   ├── energy_screen_simulator.c # LVGL PC simulator
│   └── README.md                # Simulator setup guide
├── docs/                        # Comprehensive documentation
│   ├── EMONTX3_INTEGRATION.md   # MQTT setup guide
│   ├── PEAK_TRACKING_DESIGN.md  # Peak tracking details
│   ├── MOCK_MODE_GUIDE.md       # Demo mode guide
│   └── *.md                     # Additional guides
└── README.md                    # This file
```

## 🔮 Future Enhancements

### Cumulative Display Mode
- **Daily kWh totals** instead of instantaneous power
- **Progress towards daily targets**
- **Different scales and reset timings**
- **Energy cost calculations**

### Advanced Features
- **Battery monitoring** integration
- **Weather-based solar forecasting**
- **Energy efficiency recommendations**
- **Historical trend analysis**

## 🤝 Contributing

1. **Hardware Testing**: Verify compatibility with different ESP32-S3 boards
2. **UI Improvements**: Enhance LVGL graphics and animations  
3. **MQTT Expansion**: Add support for other energy monitoring systems
4. **Documentation**: Improve setup guides and troubleshooting

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **EmonTX3**: Open-source energy monitoring platform
- **LVGL**: Lightweight graphics library  
- **ESP32 Community**: Hardware and software support
- **Waveshare**: ESP32-S3-Knob hardware platform

---

**Ready to monitor your energy in style!** 🎯⚡📊
