# SquareLine Studio Integration Guide

This project is prepared for SquareLine Studio integration. Follow these steps when you're ready to use SquareLine Studio for UI design.

## Current Setup

The code is already configured with:
- ✅ ESP32-C3 board configuration
- ✅ 360x360 display resolution
- ✅ LVGL 8.3.0 configuration
- ✅ TFT_eSPI display driver
- ✅ Touch input handling
- ✅ Proper memory allocation

## SquareLine Studio Project Settings

When creating your SquareLine Studio project, use these settings:

### Board Setup
- **Board**: ESP32
- **Display**: Custom
- **Resolution**: 360 x 360
- **Color Depth**: 16 bit
- **LVGL Version**: 8.3.0

### Project Settings
- **Project Name**: ESPKnob
- **Export Path**: `src/ui/` (create this folder)
- **UI Files**: Generate separate files for each screen

## Integration Steps

### 1. Export from SquareLine Studio
- Design your UI in SquareLine Studio
- Export project to `src/ui/` folder
- This will generate:
  - `ui.h` - Main UI header
  - `ui.c` - UI implementation
  - `ui_events.c` - Event handlers
  - `screens/` - Individual screen files

### 2. Update main.cpp
```cpp
// Uncomment this line:
#include "ui/ui.h"

// In setup(), replace:
create_demo_ui();
// With:
ui_init();
```

### 3. Add UI folder to build
Add to `platformio.ini`:
```ini
build_src_filter = +<*> +<ui/*>
```

### 4. Handle Events
Implement your custom event handlers in the generated `ui_events.c` file or create your own event handling functions.

## File Structure After Integration
```
src/
├── main.cpp              # Hardware setup & main loop
├── ui/
│   ├── ui.h              # Generated UI header
│   ├── ui.c              # Generated UI code
│   ├── ui_events.c       # Generated event handlers
│   └── screens/          # Individual screen files
└── ...
```

## Important Notes

### Memory Considerations
- Current buffer: `screenWidth * 10` lines
- For complex UIs, you might need to increase buffer size
- Monitor memory usage during development

### Touch Calibration
- Touch calibration values are set in main.cpp
- You may need to adjust these for your specific display
- Test touch accuracy with your SquareLine Studio UI

### Custom Fonts
- If using custom fonts in SquareLine Studio:
  1. Export fonts from SquareLine Studio
  2. Add font files to `src/ui/fonts/`
  3. Include font headers in your project

### Performance Tips
- Use LVGL's built-in widgets when possible
- Avoid too many animations on limited hardware
- Consider using LVGL's theme system for consistent styling

## Troubleshooting

### Build Errors
- Ensure all generated files are included in build
- Check that LVGL version matches (8.3.0)
- Verify all custom assets are properly exported

### Display Issues
- Check display orientation settings
- Verify color depth matches (16-bit)
- Ensure proper buffer size for your UI complexity

### Touch Issues
- Calibrate touch if coordinates seem off
- Check that touch events are properly connected to UI elements

## Next Steps
1. Learn SquareLine Studio basics
2. Create a simple knob interface design
3. Export and integrate with this codebase
4. Test functionality and iterate
5. Add hardware-specific features (WiFi, sensors, etc.)
