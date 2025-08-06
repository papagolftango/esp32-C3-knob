#pragma once

#include <esp_log.h>
#include <lvgl.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * Enhanced Touch Interface Manager
 * 
 * Provides advanced touch functionality including:
 * - Multi-touch gesture detection
 * - Touch calibration routines
 * - Pressure sensitivity
 * - Touch feedback integration
 * - Gesture recognition (swipe, pinch, long press)
 */
class TouchInterfaceManager {
public:
    enum TouchGesture {
        TOUCH_NONE = 0,
        TOUCH_TAP,
        TOUCH_LONG_PRESS,
        TOUCH_DOUBLE_TAP,
        TOUCH_SWIPE_LEFT,
        TOUCH_SWIPE_RIGHT,
        TOUCH_SWIPE_UP,
        TOUCH_SWIPE_DOWN,
        TOUCH_PINCH_IN,
        TOUCH_PINCH_OUT
    };
    
    struct TouchPoint {
        int16_t x;
        int16_t y;
        bool pressed;
        uint32_t pressure;
        uint32_t timestamp;
    };
    
    struct TouchEvent {
        TouchGesture gesture;
        TouchPoint start_point;
        TouchPoint end_point;
        uint32_t duration;
        int16_t distance;
        int16_t velocity;
    };
    
    typedef void (*touch_callback_t)(const TouchEvent& event);
    
public:
    /**
     * Initialize the enhanced touch interface
     */
    static void begin();
    
    /**
     * Process touch events (call regularly from main loop)
     */
    static void process();
    
    /**
     * Set callback for touch events
     */
    static void setTouchCallback(touch_callback_t callback);
    
    /**
     * Start touch calibration routine
     * Returns true if calibration completed successfully
     */
    static bool startCalibration();
    
    /**
     * Check if touch is currently pressed
     */
    static bool isTouchPressed();
    
    /**
     * Get current touch coordinates
     */
    static TouchPoint getCurrentTouch();
    
    /**
     * Enable/disable gesture recognition
     */
    static void setGestureRecognitionEnabled(bool enabled);
    
    /**
     * Set touch sensitivity (0-100)
     */
    static void setTouchSensitivity(uint8_t sensitivity);
    
    /**
     * Configure long press duration (ms)
     */
    static void setLongPressDuration(uint32_t duration_ms);
    
    /**
     * Configure double tap timeout (ms)
     */
    static void setDoubleTapTimeout(uint32_t timeout_ms);
    
    /**
     * Configure swipe threshold (pixels)
     */
    static void setSwipeThreshold(uint16_t threshold_pixels);

private:
    static bool initialized;
    static bool gesture_recognition_enabled;
    static uint8_t touch_sensitivity;
    static uint32_t long_press_duration;
    static uint32_t double_tap_timeout;
    static uint16_t swipe_threshold;
    static touch_callback_t touch_callback;
    
    // Touch state tracking
    static TouchPoint last_touch;
    static TouchPoint touch_start;
    static uint32_t touch_start_time;
    static uint32_t last_tap_time;
    static bool touch_active;
    static bool long_press_triggered;
    
    // Internal processing
    static void processRawTouch();
    static void detectGestures();
    static TouchGesture analyzeTouchSequence();
    static void triggerTouchEvent(TouchGesture gesture, const TouchPoint& start, const TouchPoint& end);
    static void performCalibrationStep(uint8_t step);
    static void saveTouchCalibration();
    static void loadTouchCalibration();
};
