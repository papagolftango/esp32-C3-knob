#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <driver/i2c_master.h>
#include <nvs_flash.h>
#include <esp_wifi.h>
#include <mqtt_client.h>

// LVGL includes
#include "lvgl.h"

// Project includes
#include "wifi_manager.h"

static const char *TAG = "ESP32_KNOB";

// Global instances
static WiFiManager wifiManager;

// WiFi status for UI
static bool wifi_connected = false;
static bool wifi_provisioning = false;

// Hardware configuration for ESP32-S3-Knob-Touch-LCD-1.8
#define ENCODER_A_PIN   GPIO_NUM_8
#define ENCODER_B_PIN   GPIO_NUM_7
#define SPI_MOSI_PIN    GPIO_NUM_2
#define SPI_SCLK_PIN    GPIO_NUM_3
#define LCD_CS_PIN      GPIO_NUM_5
#define LCD_DC_PIN      GPIO_NUM_6
#define LCD_RST_PIN     GPIO_NUM_1
#define LCD_BL_PIN      GPIO_NUM_10
#define TOUCH_CS_PIN    GPIO_NUM_0

// DRV2605 Haptic feedback (I2C)
#define I2C_SDA_PIN     GPIO_NUM_4
#define I2C_SCL_PIN     GPIO_NUM_5
#define DRV2605_ADDR    0x5A

// Display configuration
#define DISPLAY_WIDTH   360
#define DISPLAY_HEIGHT  360
#define DISPLAY_BUF_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT / 10)

// LVGL display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[DISPLAY_BUF_SIZE];
static lv_disp_drv_t disp_drv;

// Function prototypes
void init_hardware(void);
void init_display(void);
void init_lvgl(void);
void create_ui(void);
static void lvgl_task(void *pvParameters);

// WiFi callback functions
void wifi_connected_callback(void) {
    ESP_LOGI(TAG, "WiFi connected successfully");
    wifi_connected = true;
    wifi_provisioning = false;
    // Update UI to show WiFi connected status
}

void wifi_disconnected_callback(void) {
    ESP_LOGI(TAG, "WiFi disconnected");
    wifi_connected = false;
    // Update UI to show WiFi disconnected status
}

void provisioning_complete_callback(void) {
    ESP_LOGI(TAG, "WiFi provisioning completed");
    wifi_provisioning = false;
    // Update UI to show provisioning complete
}

// LVGL display flush callback
void display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
    // TODO: Implement ST77916 display driver
    // For now, just mark as ready
    lv_disp_flush_ready(disp_drv);
}

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Starting ESP32-S3 Knob Touch LCD");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize hardware
    init_hardware();
    init_display();
    init_lvgl();
    
    // Initialize WiFi Manager
    ESP_LOGI(TAG, "Initializing WiFi Manager");
    wifiManager.setCallbacks(wifi_connected_callback, 
                            wifi_disconnected_callback, 
                            provisioning_complete_callback);
    ESP_ERROR_CHECK(wifiManager.initialize());
    
    // Start WiFi provisioning or connection
    if (wifiManager.isProvisioned()) {
        ESP_LOGI(TAG, "Device is provisioned, connecting to WiFi");
        wifiManager.startProvisioning(); // This will connect to saved network
    } else {
        ESP_LOGI(TAG, "Device not provisioned, starting provisioning");
        wifi_provisioning = true;
        wifiManager.startProvisioning(); // This will start BLE provisioning
    }
    
    // Create UI
    create_ui();
    
    // Start LVGL task
    xTaskCreate(lvgl_task, "lvgl_task", 8192, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "Setup complete - ESP-IDF with LVGL and WiFi running!");
}

void init_hardware(void) {
    ESP_LOGI(TAG, "Initializing hardware for ESP32-S3-Knob-Touch-LCD-1.8");
    
    // Configure encoder pins
    gpio_config_t encoder_config = {
        .pin_bit_mask = (1ULL << ENCODER_A_PIN) | (1ULL << ENCODER_B_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&encoder_config);
    
    // Configure display pins
    gpio_config_t display_config = {
        .pin_bit_mask = (1ULL << LCD_RST_PIN) | (1ULL << LCD_BL_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
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

void init_display(void) {
    ESP_LOGI(TAG, "Initializing ST77916 display driver");
    
    // TODO: Initialize SPI and ST77916 display driver
    // This would include SPI configuration and ST77916 specific commands
    
    ESP_LOGI(TAG, "Display driver initialized");
}

void init_lvgl(void) {
    ESP_LOGI(TAG, "Initializing LVGL");
    
    lv_init();
    
    // Initialize display buffer
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, DISPLAY_BUF_SIZE);
    
    // Initialize display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = DISPLAY_WIDTH;
    disp_drv.ver_res = DISPLAY_HEIGHT;
    disp_drv.flush_cb = display_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
    ESP_LOGI(TAG, "LVGL initialized");
}

void create_ui(void) {
    ESP_LOGI(TAG, "Creating UI with WiFi status");
    
    // Create main label
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "ESP32-S3 Knob\nESP-IDF + LVGL\n+ WiFiManager");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -60);
    
    // Create WiFi status label
    lv_obj_t *wifi_label = lv_label_create(lv_scr_act());
    if (wifi_provisioning) {
        lv_label_set_text(wifi_label, "WiFi: Provisioning...\nUse ESP WiFi Prov app");
        lv_obj_set_style_text_color(wifi_label, lv_color_hex(0xFFA500), 0); // Orange
    } else if (wifi_connected) {
        lv_label_set_text(wifi_label, "WiFi: Connected");
        lv_obj_set_style_text_color(wifi_label, lv_color_hex(0x00FF00), 0); // Green
    } else {
        lv_label_set_text(wifi_label, "WiFi: Disconnected");
        lv_obj_set_style_text_color(wifi_label, lv_color_hex(0xFF0000), 0); // Red
    }
    lv_obj_set_style_text_font(wifi_label, &lv_font_montserrat_18, 0);
    lv_obj_align(wifi_label, LV_ALIGN_CENTER, 0, 0);
    
    // Create an arc widget
    lv_obj_t *arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(arc, 150, 150);
    lv_obj_align(arc, LV_ALIGN_CENTER, 0, 60);
    lv_arc_set_value(arc, 50);
    
    ESP_LOGI(TAG, "UI created successfully");
}

static void lvgl_task(void *pvParameters) {
    ESP_LOGI(TAG, "Starting LVGL task");
    
    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
