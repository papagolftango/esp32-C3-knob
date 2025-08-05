#include "hardware.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

static const char* TAG = "BUTTON_TEST";

void test_button_hardware(void) {
    ESP_LOGI(TAG, "=== BUTTON HARDWARE TEST ===");
    
    // Configure button pin as input with pullup
    gpio_config_t button_config = {
        .pin_bit_mask = (1ULL << ENCODER_BTN_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&button_config);
    
    ESP_LOGI(TAG, "Button test on GPIO_%d", ENCODER_BTN_PIN);
    ESP_LOGI(TAG, "Expected: HIGH (1) when not pressed, LOW (0) when pressed");
    ESP_LOGI(TAG, "Push the button to test...");
    
    int last_state = -1;
    int press_count = 0;
    
    for (int i = 0; i < 100; i++) {  // Test for 10 seconds
        int current_state = gpio_get_level(ENCODER_BTN_PIN);
        
        if (current_state != last_state) {
            if (current_state == 0) {
                press_count++;
                ESP_LOGI(TAG, "BUTTON PRESS %d detected! GPIO_%d = %d", 
                         press_count, ENCODER_BTN_PIN, current_state);
            } else {
                ESP_LOGI(TAG, "BUTTON RELEASE detected! GPIO_%d = %d", 
                         ENCODER_BTN_PIN, current_state);
            }
            last_state = current_state;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));  // Check every 100ms
    }
    
    ESP_LOGI(TAG, "Test complete. Button presses detected: %d", press_count);
    
    if (press_count == 0) {
        ESP_LOGW(TAG, "NO button presses detected!");
        ESP_LOGW(TAG, "This could mean:");
        ESP_LOGW(TAG, "  1. No button is connected to GPIO_%d", ENCODER_BTN_PIN);
        ESP_LOGW(TAG, "  2. Button is connected to different GPIO");
        ESP_LOGW(TAG, "  3. Hardware doesn't have a button");
    } else {
        ESP_LOGI(TAG, "Button hardware is working correctly!");
    }
}
