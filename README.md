# ESP32-S3 Knob Touch LCD

Smart home control panel with round touch display for energy monitoring and home automation.

![ESP32-S3](https://img.shields.io/badge/Platform-ESP32--S3-blue)
![ESP-IDF](https://img.shields.io/badge/Framework-ESP--IDF%20v5.5-green)
![LVGL](https://img.shields.io/badge/Graphics-LVGL%208.3.0-purple)
![Display](https://img.shields.io/badge/Display-360x360%20ST77916-orange)

## What It Does

- **Energy monitoring** - Shows real-time power usage, daily peaks, and kWh totals
- **Bin collection** - Displays which bins are due for collection with visual reminders
- **News ticker** - Scrolling messages from your home automation system
- **WiFi setup** - Easy configuration via web interface
- **MQTT integration** - Connects to your existing smart home setup
- **Touch interface** - Navigate between screens with touch or rotary encoder

## Getting Started

### What You Need
- ESP32-S3-Knob-Touch-LCD-1.8 board
- USB-C cable
- Computer with ESP-IDF v5.5

### Installation
1. **Get the code:**
   ```bash
   git clone https://github.com/papagolftango/ESP32-C3-Knob.git
   cd ESP32-C3-Knob
   ```

2. **Setup ESP-IDF (Windows):**
   ```bash
   & "C:\Espressif\frameworks\esp-idf-v5.5\export.ps1"
   ```

3. **Build and flash:**
   ```bash
   idf.py build
   idf.py flash monitor
   ```

### Setup WiFi
1. Power on the device
2. Connect to `ESP32-Knob-Setup` (password: `knob123`)
3. Configure your home WiFi and MQTT server in the web interface
4. Device restarts and connects automatically

## Status Indicators

- **Green arc** = Connected and working  
- **Orange arc** = WiFi connected, MQTT offline
- **Red arc** = WiFi disconnected

## MQTT Topics

Send data to your device using these MQTT topics:

| Topic | Purpose | Example |
|-------|---------|---------|
| `home/energy/import` | Current power usage (watts) | `2340` |
| `home/energy/solar` | Solar generation (watts) | `1580` |
| `home/bins/tonight` | Bins for collection | `general,recycling` |
| `home/motd` | News ticker message | `Welcome home!` |

## More Information

- **[WiFi Setup](WIFI_SETUP.md)** - Detailed WiFi configuration
- **[MQTT Setup](MQTT_SETUP.md)** - Connect to your MQTT broker  
- **[MQTT Topics](HOME_KNOB_TOPICS_GUIDE.md)** - Complete topic reference
- **[Hardware Info](README_ESP_IDF.md)** - Technical specifications

### Memory Usage
- **Flash:** ~1.64MB (of 4MB available) - 21% usage
- **RAM:** ~48KB LVGL buffer + application
- **PSRAM:** Not required for basic operation

## 🎯 Use Cases

- **Smart home controls** - HVAC, lighting, music
- **Energy monitoring** - Real-time consumption tracking with daily peaks
- **Household management** - Bin collection scheduling and notifications
- **Information display** - Scrolling news ticker and status updates
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
- [ESP-IDF](https://idf.espressif.com/) - Official ESP32 development framework
- [Espressif](https://www.espressif.com/) - ESP32 hardware and software
- [SquareLine Studio](https://squareline.io/) - UI design tool

## 📞 Support

- **Issues:** [GitHub Issues](https://github.com/yourusername/ESP32-C3-Knob/issues)
- **Discussions:** [GitHub Discussions](https://github.com/yourusername/ESP32-C3-Knob/discussions)
- **Documentation:** See markdown files in this repository

---

**Made with ❤️ for the maker community**
