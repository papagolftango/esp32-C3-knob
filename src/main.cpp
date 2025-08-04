#include <Arduino.h>
#include <lvgl.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Testing LVGL font inclusion...");
    
    // Initialize LVGL
    lv_init();
    
    // Test font references
    const lv_font_t* font14 = &lv_font_montserrat_14;
    const lv_font_t* font16 = &lv_font_montserrat_16;
    const lv_font_t* font18 = &lv_font_montserrat_18;
    
    Serial.println("Font references working!");
    Serial.printf("Font 14 height: %d\n", font14->line_height);
}

void loop() {
    delay(1000);
}
