#pragma once

#include <esp_log.h>
#include <esp_driver_i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * Enhanced Haptic Feedback Manager
 * 
 * Provides comprehensive haptic feedback using DRV2605 haptic driver
 * Features:
 * - Multiple haptic patterns and intensities
 * - Context-aware feedback (navigation, confirmation, error, etc.)
 * - Custom haptic sequences
 * - Adaptive feedback based on user preferences
 * - Integration with touch and navigation systems
 */
class HapticFeedbackManager {
public:
    enum HapticPattern {
        // Basic feedback
        HAPTIC_LIGHT_CLICK = 0,
        HAPTIC_MEDIUM_CLICK,
        HAPTIC_STRONG_CLICK,
        HAPTIC_DOUBLE_CLICK,
        
        // Navigation feedback
        HAPTIC_MENU_NAVIGATE,
        HAPTIC_SCREEN_CHANGE,
        HAPTIC_BUTTON_PRESS,
        HAPTIC_SCROLL_TICK,
        
        // Status feedback
        HAPTIC_SUCCESS,
        HAPTIC_WARNING,
        HAPTIC_ERROR,
        HAPTIC_NOTIFICATION,
        
        // Special patterns
        HAPTIC_STARTUP,
        HAPTIC_SHUTDOWN,
        HAPTIC_FACTORY_RESET,
        HAPTIC_PEAK_REACHED,
        
        // Custom patterns
        HAPTIC_CUSTOM_1,
        HAPTIC_CUSTOM_2,
        HAPTIC_CUSTOM_3
    };
    
    enum HapticIntensity {
        INTENSITY_LIGHT = 0,
        INTENSITY_MEDIUM,
        INTENSITY_STRONG,
        INTENSITY_MAXIMUM
    };
    
    struct HapticSequence {
        HapticPattern pattern;
        uint16_t duration_ms;
        uint16_t pause_ms;
        uint8_t repeat_count;
        HapticIntensity intensity;
    };

public:
    /**
     * Initialize the haptic feedback system
     */
    static void begin();
    
    /**
     * Check if haptic feedback is available and enabled
     */
    static bool isAvailable();
    
    /**
     * Enable/disable all haptic feedback
     */
    static void setEnabled(bool enabled);
    
    /**
     * Check if haptic feedback is enabled
     */
    static bool isEnabled();
    
    /**
     * Play a predefined haptic pattern
     */
    static void playPattern(HapticPattern pattern);
    
    /**
     * Play a custom haptic sequence
     */
    static void playSequence(const HapticSequence* sequence, uint8_t sequence_length);
    
    /**
     * Set global haptic intensity (0-100)
     */
    static void setGlobalIntensity(uint8_t intensity);
    
    /**
     * Get current global intensity
     */
    static uint8_t getGlobalIntensity();
    
    // Context-specific convenience methods
    static void navigationFeedback();      // Light click for menu navigation
    static void confirmationFeedback();    // Double click for confirmations
    static void errorFeedback();           // Strong buzz for errors
    static void successFeedback();         // Pleasant pulse for success
    static void screenChangeFeedback();    // Medium click for screen changes
    static void buttonPressFeedback();     // Quick click for button presses
    static void notificationFeedback();    // Gentle buzz for notifications
    static void peakReachedFeedback();     // Sharp pulse for energy peaks
    static void factoryResetFeedback();    // Special sequence for factory reset
    
    /**
     * Test all haptic patterns (useful for setup/debugging)
     */
    static void testAllPatterns();
    
    /**
     * Create custom haptic pattern
     */
    static void defineCustomPattern(HapticPattern pattern_id, const HapticSequence& sequence);

private:
    static bool initialized;
    static bool enabled;
    static bool device_available;
    static uint8_t global_intensity;
    static i2c_master_dev_handle_t drv2605_handle;
    
    // DRV2605 register definitions
    static const uint8_t DRV2605_ADDR = 0x5A;
    static const uint8_t DRV2605_STATUS = 0x00;
    static const uint8_t DRV2605_MODE = 0x01;
    static const uint8_t DRV2605_LIBRARY = 0x03;
    static const uint8_t DRV2605_WAVESEQ1 = 0x04;
    static const uint8_t DRV2605_GO = 0x0C;
    
    // Haptic effect library (DRV2605 built-in effects)
    static const uint8_t EFFECT_LIGHT_CLICK = 14;
    static const uint8_t EFFECT_MEDIUM_CLICK = 10;
    static const uint8_t EFFECT_STRONG_CLICK = 1;
    static const uint8_t EFFECT_DOUBLE_CLICK = 2;
    static const uint8_t EFFECT_BUZZ_SHORT = 17;
    static const uint8_t EFFECT_PULSE_SHARP = 47;
    static const uint8_t EFFECT_ERROR_BUZZ = 36;
    
    // Internal methods
    static bool initializeHardware();
    static void writeRegister(uint8_t reg, uint8_t value);
    static uint8_t readRegister(uint8_t reg);
    static void playEffect(uint8_t effect_id);
    static void playEffectWithIntensity(uint8_t effect_id, HapticIntensity intensity);
    static uint8_t mapPatternToEffect(HapticPattern pattern);
    static void configureIntensity(HapticIntensity intensity);
};
