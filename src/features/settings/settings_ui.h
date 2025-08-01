#pragma once

#include <lvgl.h>
#include "../../core/hardware/haptic_feedback.h"

class SettingsUI {
public:
    // Initialize settings UI system
    static void begin();
    
    // Update the settings screen display
    static void updateScreen();
    
    // Handle encoder rotation in settings menu
    static void handleEncoderRotation(int direction);
    
    // Handle touch/selection in settings menu
    static void handleSelection();
    
    // Check if settings menu is in navigation mode
    static bool isMenuActive();
    
    // Activate/deactivate settings menu navigation
    static void setMenuActive(bool active);
    
    // Reset settings menu to first item
    static void reset();
    
private:
    static int selected_item;
    static bool menu_active;
    static const int MENU_ITEMS = 2;  // Haptic Feedback, WiFi Reset
    
    // Helper methods
    static void createScreen();
    static void handleHapticToggle();
    static void handleWiFiReset();
};
