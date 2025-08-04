# ESP32-S3 Knob Project - ESP-IDF Version

This project has been converted from PlatformIO to ESP-IDF for better ESP32-S3 support and more reliable compilation.

## Hardware
- **Board**: Waveshare ESP32-S3-Knob-Touch-LCD-1.8
- **Display**: 360×360 ST77916 QSPI LCD
- **Encoder**: GPIO 7/8 (rotation only)
- **Haptic**: DRV2605 I2C vibration motor driver
- **Touch**: CST816 I2C touch controller

## Prerequisites

1. **Install ESP-IDF**: Follow the [ESP-IDF Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/)
2. **Set up environment**: Make sure `IDF_PATH` is set and ESP-IDF tools are in your PATH

## Building and Flashing

### Option 1: Using ESP-IDF Command Line
```bash
# Set up the environment (run this in each new terminal)
. $IDF_PATH/export.sh   # Linux/macOS
# OR
%IDF_PATH%\\export.bat  # Windows

# Configure the project
idf.py set-target esp32s3
idf.py menuconfig

# Build the project
idf.py build

# Flash to device
idf.py -p COM_PORT flash monitor
```

### Option 2: Using VS Code ESP-IDF Extension
1. Install the "ESP-IDF" extension in VS Code
2. Press `Ctrl+Shift+P` and run "ESP-IDF: Configure ESP-IDF extension"
3. Use the ESP-IDF commands from the command palette:
   - "ESP-IDF: Set Espressif device target"
   - "ESP-IDF: Build your project"
   - "ESP-IDF: Flash your project"

## Project Structure

```
ESPKnob/
├── CMakeLists.txt              # Main project CMake file
├── sdkconfig.defaults          # Default configuration
├── main/
│   ├── CMakeLists.txt         # Main component CMake
│   └── main.cpp               # Main application code
├── components/
│   └── lv_conf.h              # LVGL configuration
└── README_ESP_IDF.md          # This file
```

## Key Features

### LVGL Integration
- Configured for ESP32-S3 with PSRAM support
- Optimized memory usage (64KB LVGL buffer)
- Font support: Montserrat 12, 14, 16, 18
- Arc and label widgets enabled

### Hardware Support
- ESP32-S3 240MHz with 8MB PSRAM
- ST77916 display driver framework
- Rotary encoder on GPIO 7/8
- I2C haptic feedback (DRV2605)
- WiFi and MQTT ready

### ESP-IDF Advantages
- Better ESP32-S3 hardware support
- More reliable compilation
- Official Espressif toolchain
- Better debugging capabilities
- Direct access to ESP-IDF components

## Configuration

### Display Driver
The ST77916 display driver needs to be implemented in the `display_flush()` function. The current version includes the framework.

### WiFi and MQTT
The project includes WiFi provisioning and MQTT client setup, ready for EmonTX3 integration.

### LVGL Configuration
LVGL is configured in `components/lv_conf.h` with:
- 16-bit color depth
- 64KB memory allocation
- Essential widgets enabled
- Montserrat fonts 12-18

## Troubleshooting

### Build Issues
- Make sure ESP-IDF is properly installed and environment is set up
- Run `idf.py clean` if you encounter cache issues
- Check that `IDF_PATH` environment variable is set

### Flash Issues
- Ensure the correct COM port is specified
- Try holding the BOOT button while flashing
- Check that the ESP32-S3 is properly connected

### Memory Issues
- The project is configured for ESP32-S3 with PSRAM
- Adjust `LV_MEM_SIZE` in `lv_conf.h` if needed
- Monitor heap usage with `idf.py monitor`

## Next Steps

1. **Implement ST77916 Driver**: Complete the display driver implementation
2. **Add Touch Support**: Implement CST816 touch controller
3. **Haptic Feedback**: Complete DRV2605 integration
4. **WiFi/MQTT**: Connect to EmonTX3 energy monitoring
5. **UI Development**: Create the energy monitoring interface

This ESP-IDF version provides a solid foundation for your ESP32-S3 knob project with better hardware support and more reliable compilation than PlatformIO.
