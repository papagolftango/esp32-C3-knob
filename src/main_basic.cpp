#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Basic ESP32-S3 test - no LVGL");
}

void loop() {
    Serial.println("Running...");
    delay(1000);
}
