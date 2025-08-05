#include "hardware.h"
#include "rotary.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG = "HARDWARE";

void hardware_init(void) {
    ESP_LOGI(TAG, "Initializing hardware");
    
    // Install GPIO ISR service
    gpio_install_isr_service(0);
    
    hardware_init_rotary();
    hardware_init_spi();
    hardware_init_i2c();
    
    // Initialize display control pins
    gpio_config_t display_config = {
        .pin_bit_mask = (1ULL << LCD_CS_PIN) | (1ULL << LCD_DC_PIN) | 
                       (1ULL << LCD_RST_PIN) | (1ULL << LCD_BL_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&display_config);
    
    // Reset display
    gpio_set_level(LCD_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(LCD_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    // Turn on backlight
    gpio_set_level(LCD_BL_PIN, 1);
    
    ESP_LOGI(TAG, "Hardware initialization complete");
}

void hardware_init_rotary(void) {
    ESP_LOGI(TAG, "Initializing rotary encoder");
    
    // Configure rotary encoder
    rotary_config_t rotary_config = {
        .pin_a = ENCODER_A_PIN,
        .pin_b = ENCODER_B_PIN,
        .pin_btn = ENCODER_BTN_PIN,
        .invert_direction = false,
        .debounce_ms = 5
    };
    
    esp_err_t ret = rotary_init(&rotary_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize rotary encoder: %s", esp_err_to_name(ret));
        return;
    }
    
    // Start rotary encoder task
    ret = rotary_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start rotary encoder task: %s", esp_err_to_name(ret));
        return;
    }
    
    ESP_LOGI(TAG, "Rotary encoder initialized and started");
}

void hardware_init_spi(void) {
    ESP_LOGI(TAG, "Initializing SPI for display");
    
    // TODO: Initialize SPI bus for ST77916 display
    // This will be implemented when we add the display driver
    
    ESP_LOGI(TAG, "SPI initialized");
}

void hardware_init_i2c(void) {
    ESP_LOGI(TAG, "Initializing I2C for haptic feedback");
    
    // TODO: Initialize I2C bus for DRV2605 haptic driver
    // This will be implemented when we add haptic feedback
    
    ESP_LOGI(TAG, "I2C initialized");
}
