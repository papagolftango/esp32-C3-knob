#pragma once
#include <Arduino.h>

// Haptic effect definitions
enum HapticEffect {
    HAPTIC_CLICK_STRONG = 1,      // Strong click (screen change)
    HAPTIC_CLICK_MEDIUM = 10,     // Medium click (encoder step)
    HAPTIC_CLICK_LIGHT = 14,      // Light click (touch)
    HAPTIC_BUZZ_SHORT = 17,       // Short buzz (notification)
    HAPTIC_PULSE_SHARP = 47,      // Sharp pulse (peak reached)
    HAPTIC_DOUBLE_CLICK = 2,      // Double click (confirmation)
    HAPTIC_ERROR = 36             // Error vibration
};

class HapticFeedback {
public:
    static void begin();
    static void setEnabled(bool enabled);
    static bool isEnabled();
    static void playEffect(HapticEffect effect);
    
    // Convenience functions
    static void click();
    static void screenChange();
    static void peakReached();
    static void touch();
    static void error();
    static void confirmation();

private:
    static bool haptic_enabled;
    static void writeRegister(uint8_t reg, uint8_t value);
    static uint8_t readRegister(uint8_t reg);
    static void initialize();
};
