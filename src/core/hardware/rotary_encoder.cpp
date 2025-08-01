#include "rotary_encoder.h"
#include "haptic_feedback.h"

volatile bool RotaryEncoder::encoder_a_state = false;
volatile bool RotaryEncoder::encoder_b_state = false;
volatile int RotaryEncoder::encoder_position = 0;
volatile bool RotaryEncoder::encoder_direction = false;
volatile unsigned long RotaryEncoder::last_encoder_time = 0;
std::function<void(int)> RotaryEncoder::navigation_callback = nullptr;

void RotaryEncoder::begin() {
    pinMode(ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(ENCODER_PIN_B, INPUT_PULLUP);
    
    // Attach interrupts
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoder_isr, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), encoder_isr, CHANGE);
    
    Serial.printf("Rotary encoder initialized - A:%d B:%d\n", ENCODER_PIN_A, ENCODER_PIN_B);
}

void RotaryEncoder::setNavigationCallback(std::function<void(int)> callback) {
    navigation_callback = callback;
}

void RotaryEncoder::handleNavigation() {
    static int last_position = 0;
    static unsigned long last_nav_time = 0;
    unsigned long now = millis();
    
    // Check for position changes (with some hysteresis to avoid noise)
    int position_change = encoder_position - last_position;
    
    if (abs(position_change) >= 2 && (now - last_nav_time > 150)) { // Require 2 steps and debounce
        if (navigation_callback) {
            navigation_callback(position_change);
        }
        
        last_position = encoder_position;
        last_nav_time = now;
    } else if (abs(position_change) >= 1 && (now - last_nav_time > 50)) {
        // Smaller rotations give lighter feedback (without screen change)
        HapticFeedback::click();
        last_position = encoder_position;
        last_nav_time = now;
    }
}

int RotaryEncoder::getPositionChange() {
    static int last_position = 0;
    int change = encoder_position - last_position;
    last_position = encoder_position;
    return change;
}

void IRAM_ATTR RotaryEncoder::encoder_isr() {
    unsigned long now = millis();
    if (now - last_encoder_time < 2) return; // Simple debouncing
    
    bool a_state = digitalRead(ENCODER_PIN_A);
    bool b_state = digitalRead(ENCODER_PIN_B);
    
    // Detect rotation direction using quadrature encoding
    if (a_state != encoder_a_state) {
        if (a_state != b_state) {
            encoder_position++;
            encoder_direction = true; // CW
        } else {
            encoder_position--;
            encoder_direction = false; // CCW
        }
    }
    
    encoder_a_state = a_state;
    encoder_b_state = b_state;
    last_encoder_time = now;
}
