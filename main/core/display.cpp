#include "display.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG = "DISPLAY";

// LVGL display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[DISPLAY_BUF_SIZE];
static lv_disp_drv_t disp_drv;

static void lvgl_task(void *pvParameters);

void display_init(void) {
    ESP_LOGI(TAG, "Initializing LVGL and display driver");
    
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
    
    ESP_LOGI(TAG, "LVGL and display driver initialized");
}

void display_start_task(void) {
    ESP_LOGI(TAG, "Starting LVGL task");
    
    xTaskCreate(lvgl_task, "lvgl_task", 4096, NULL, 5, NULL);
}

void display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
    // TODO: Implement ST77916 display driver
    // For now, just mark as ready
    lv_disp_flush_ready(disp_drv);
}

static void lvgl_task(void *pvParameters) {
    ESP_LOGI(TAG, "LVGL task started");
    
    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
