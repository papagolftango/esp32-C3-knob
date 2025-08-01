# ESP32-C3 Knob Display

A professional knob interface built on ESP32-C3 with 360x360 round display, featuring LVGL graphics, WiFiManager, and SquareLine Studio support.

![ESP32-C3 Knob](https://img.shields.io/badge/Platform-ESP32--C3-blue)
![LVGL](https://img.shields.io/badge/Graphics-LVGL%208.3.0-green)
![WiFi](https://img.shields.io/badge/WiFi-Manager-orange)
![Display](https://img.shields.io/badge/Display-360x360-purple)

## 🎛️ Features

- **ESP32-C3** microcontroller with WiFi/Bluetooth
- **360x360 round display** with ST7789 driver
- **Capacitive touch screen** support
- **LVGL 8.3.0** graphics library integration
- **WiFiManager** for secure credential storage
- **SquareLine Studio** ready for advanced UI design
- **Professional code structure** for production use

## 🚀 Quick Start

### Hardware Requirements
- Waveshare ESP32-C3 1.8" knob display (360x360)
- USB-C cable for programming
- Computer with PlatformIO installed

### Software Setup
1. **Clone this repository:**
   ```bash
   git clone https://github.com/yourusername/ESP32-C3-Knob.git
   cd ESP32-C3-Knob
   ```

2. **Open in VS Code with PlatformIO extension**

3. **Build and upload:**
   ```bash
   pio run --target upload --target monitor
   ```

### WiFi Configuration
1. Power on the device
2. Connect to WiFi network: `ESP32-Knob-Setup` (password: `knob123`)
3. Browser opens automatically for configuration
4. Select your home WiFi and enter password
5. Device restarts and connects automatically

## 📱 Visual Feedback

- **Green arc** = WiFi connected ✅
- **Red arc** = WiFi disconnected ❌  
- **IP address** displayed when connected
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
