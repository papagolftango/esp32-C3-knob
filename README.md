# ESP32-S3 Knob Touch LCD

A professional knob interface built on ESP32-S3-Knob-Touch-LCD-1.8 with 360x360 round display, featuring LVGL graphics, WiFi connectivity, and EmonTX3 energy monitoring integration.

![ESP32-S3](https://img.shields.io/badge/Platform-ESP32--S3-blue)
![ESP-IDF](https://img.shields.io/badge/Framework-ESP--IDF%20v5.5-green)
![LVGL](https://img.shields.io/badge/Graphics-LVGL%208.3.0-purple)
![Display](https://img.shields.io/badge/Display-360x360%20ST77916-orange)

## 🎛️ Features

- **ESP32-S3 dual MCU** (ESP32-S3R8 + ESP32-U4WDH) with WiFi/Bluetooth
- **360x360 round display** with ST77916 driver and capacitive touch
- **DRV2605 haptic feedback** for tactile interactions
- **Rotary encoder** for precise navigation (GPIO7/8)
- **LVGL 8.3.0** graphics with multi-screen UI system
- **EmonTX3 integration** for real-time energy monitoring
- **WiFiManager** for secure credential storage (no hardcoded passwords)
- **MQTT client** with auto-reconnection and topic routing
- **Peak tracking** with daily reset functionality
- **Mock mode** for development without EmonTX3 hardware
- **Professional architecture** with feature-based component structure

## 🚀 Quick Start

### Hardware Requirements
- **ESP32-S3-Knob-Touch-LCD-1.8** development board
- **USB-C cable** for programming and power
- **Computer** with ESP-IDF v5.5 installed

### Software Setup
1. **Clone this repository:**
   ```bash
   git clone https://github.com/papagolftango/ESP32-C3-Knob.git
   cd ESP32-C3-Knob
   ```

2. **Set up ESP-IDF environment:**
   ```bash
   # Windows PowerShell
   & "C:\Espressif\frameworks\esp-idf-v5.5\export.ps1"
   
   # Or use ESP-IDF PowerShell shortcut from Start Menu
   ```

3. **Build and flash:**
   ```bash
   idf.py set-target esp32s3
   idf.py build
   idf.py flash monitor
   ```

### VS Code Integration
- Install **ESP-IDF extension** for VS Code
- Use **Command Palette** (`Ctrl+Shift+P`) → "ESP-IDF: Flash"
- Or run **VS Code tasks**: "ESP-IDF: Flash and Monitor"
   ```

### WiFi Configuration
1. Power on the device
2. Connect to WiFi network: `ESP32-Knob-Setup` (password: `knob123`)
3. Browser opens automatically for configuration
4. Select your home WiFi and enter password
5. **Configure MQTT settings** (server, port, username, password)
6. Device restarts and connects automatically

📖 **Detailed Guides:**
- [WiFi Setup Guide](WIFI_SETUP.md) - Complete WiFi configuration instructions
- [MQTT Setup Guide](MQTT_SETUP.md) - MQTT broker setup and integration

## 📱 Visual Feedback

- **🟢 Green arc** = WiFi + MQTT connected ✅  
- **🟠 Orange arc** = WiFi connected, MQTT disconnected ⚠️
- **🔴 Red arc** = WiFi disconnected ❌  
- **Connection details** displayed on screen (IP, MQTT broker)
- **Touch responsive** interface

## 🏗️ Project Structure

```
ESP32-C3-Knob/
├── src/
│   └── main.cpp              # Main application code
├── include/
│   ├── lv_conf.h            # LVGL configuration
│   └── User_Setup.h         # TFT_eSPI display config
├── platformio.ini           # Build configuration
├── .gitignore              # Git ignore rules
├── WIFI_SETUP.md           # WiFi setup guide
├── SQUARELINE_INTEGRATION.md # SquareLine Studio guide
└── README.md               # This file
```

## 🔧 Configuration

### Display Settings
- **Resolution:** 360x360 pixels
- **Driver:** ST7789
- **Interface:** SPI
- **Touch:** Capacitive

### Pin Configuration (ESP32-C3)
```cpp
#define TFT_MOSI  2    // SPI Data
#define TFT_SCLK  3    // SPI Clock  
#define TFT_CS    5    // Chip Select
#define TFT_DC    6    // Data/Command
#define TFT_RST   1    // Reset
#define TFT_BL    10   // Backlight
#define TOUCH_CS  0    # Touch CS
```

## 🎨 SquareLine Studio Integration

This project is prepared for SquareLine Studio UI design:

1. **Create new project** with these settings:
   - Board: ESP32
   - Resolution: 360 x 360
   - Color Depth: 16 bit
   - LVGL Version: 8.3.0

2. **Export to** `src/ui/` folder

3. **Update main.cpp:**
   ```cpp
   #include "ui/ui.h"  // Uncomment this line
   ui_init();          // Replace create_demo_ui();
   ```

See [SQUARELINE_INTEGRATION.md](SQUARELINE_INTEGRATION.md) for detailed instructions.

## 🔒 WiFi Security

- ✅ No hardcoded credentials in source code
- ✅ Encrypted storage in ESP32 flash
- ✅ Captive portal for easy setup
- ✅ Multiple network support
- ✅ Automatic reconnection

See [WIFI_SETUP.md](WIFI_SETUP.md) for advanced configuration.

## 📚 Dependencies

All dependencies are automatically managed by PlatformIO:

- `bodmer/TFT_eSPI@^2.5.0` - Display driver
- `lvgl/lvgl@^8.3.0` - Graphics library  
- `tzapu/WiFiManager@^0.16.0` - WiFi configuration
- `Wire` - I2C communication

## 🛠️ Development

### Building
```bash
pio run                    # Build project
pio run -t upload          # Upload to device
pio run -t monitor         # Open serial monitor
pio run -t uploadfs        # Upload filesystem (if used)
```

### Debugging
```bash
pio device monitor         # Serial monitor
pio run -t clean           # Clean build files
```

### Memory Usage
- **Flash:** ~1.2MB (of 4MB available)
- **RAM:** ~48KB LVGL buffer + application
- **PSRAM:** Not required for basic operation

## 🎯 Use Cases

- **Smart home controls** - HVAC, lighting, music
- **Industrial interfaces** - Machine control panels
- **Audio equipment** - Volume, EQ, mixing controls  
- **IoT dashboards** - Sensor monitoring and control
- **Gaming controllers** - Custom input devices

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- [LVGL](https://lvgl.io/) - Amazing graphics library
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) - Display driver
- [WiFiManager](https://github.com/tzapu/WiFiManager) - WiFi configuration
- [PlatformIO](https://platformio.org/) - Development platform
- [SquareLine Studio](https://squareline.io/) - UI design tool

## 📞 Support

- **Issues:** [GitHub Issues](https://github.com/yourusername/ESP32-C3-Knob/issues)
- **Discussions:** [GitHub Discussions](https://github.com/yourusername/ESP32-C3-Knob/discussions)
- **Documentation:** See markdown files in this repository

---

**Made with ❤️ for the maker community**
