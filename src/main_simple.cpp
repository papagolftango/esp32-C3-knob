#include <Arduino.h>
#include <lvgl.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Testing LVGL compilation...");
    
    // Test if LVGL types are recognized
    lv_obj_t* test_obj = nullptr;
    Serial.println("LVGL types working!");
}

void loop() {
    delay(1000);
}
