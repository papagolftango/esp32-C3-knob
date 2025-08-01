#pragma once
#include <Arduino.h>
#include <functional>

class RotaryEncoder {
public:
    static void begin();
    static void setNavigationCallback(std::function<void(int)> callback);
    static void handleNavigation();
    
    // Get encoder position change since last check
    static int getPositionChange();
    
private:
    static constexpr int ENCODER_PIN_A = 8;   // GPIO8
    static constexpr int ENCODER_PIN_B = 7;   // GPIO7
    
    static volatile bool encoder_a_state;
    static volatile bool encoder_b_state;
    static volatile int encoder_position;
    static volatile bool encoder_direction;
    static volatile unsigned long last_encoder_time;
    
    static std::function<void(int)> navigation_callback;
    
    static void IRAM_ATTR encoder_isr();
};
