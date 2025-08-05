#include "rotary.h"
#include "hardware.h"
#include <esp_log.h>
#include <freertos/timers.h>
#include <driver/gpio.h>
#include <string.h>

static const char *TAG = "ROTARY";

// Rotary encoder state
static struct {
    rotary_config_t config;
    bool initialized;
    bool running;
    TaskHandle_t task_handle;
    QueueHandle_t event_queue;
    rotary_event_callback_t callback;
    
    // Encoder state tracking
    uint8_t last_state;
    uint32_t last_change_time;
    bool button_pressed;
    uint32_t button_press_time;
} rotary_ctx;

// Forward declarations
static void rotary_task(void *pvParameters);
static void rotary_isr_handler(void *arg);
static void button_isr_handler(void *arg);
static uint8_t rotary_read_pins(void);

esp_err_t rotary_init(const rotary_config_t *config) {
    if (config == NULL) {
        ESP_LOGE(TAG, "Config cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "Initializing rotary encoder");
    
    // Initialize context
    memset(&rotary_ctx, 0, sizeof(rotary_ctx));
    
    // Copy configuration
    rotary_ctx.config = *config;
    
    // Create event queue
    rotary_ctx.event_queue = xQueueCreate(10, sizeof(rotary_event_t));
    if (rotary_ctx.event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create event queue");
        return ESP_ERR_NO_MEM;
    }
    
    // Configure encoder pins
    gpio_config_t encoder_config = {
        .pin_bit_mask = (1ULL << config->pin_a) | (1ULL << config->pin_b),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    gpio_config(&encoder_config);
    
    // Configure button pin if specified
    if (config->pin_btn != GPIO_NUM_NC) {
        gpio_config_t button_config = {
            .pin_bit_mask = (1ULL << config->pin_btn),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_NEGEDGE, // Button press (falling edge)
        };
        gpio_config(&button_config);
        
        // Install button ISR handler
        gpio_isr_handler_add(config->pin_btn, button_isr_handler, NULL);
    }
    
    // Install encoder ISR handlers
    gpio_isr_handler_add(config->pin_a, rotary_isr_handler, NULL);
    gpio_isr_handler_add(config->pin_b, rotary_isr_handler, NULL);
    
    // Initialize state
    rotary_ctx.last_state = rotary_read_pins();
    rotary_ctx.last_change_time = xTaskGetTickCount();
    rotary_ctx.button_pressed = false;
    rotary_ctx.initialized = true;
    
    ESP_LOGI(TAG, "Rotary encoder initialized (pins A:%d, B:%d, BTN:%d)", 
             config->pin_a, config->pin_b, config->pin_btn);
    
    return ESP_OK;
}

void rotary_set_callback(rotary_event_callback_t callback) {
    rotary_ctx.callback = callback;
}

bool rotary_get_event(rotary_event_t *event) {
    if (event == NULL || rotary_ctx.event_queue == NULL) {
        return false;
    }
    
    return xQueueReceive(rotary_ctx.event_queue, event, 0) == pdTRUE;
}

bool rotary_wait_event(rotary_event_t *event, uint32_t timeout_ms) {
    if (event == NULL || rotary_ctx.event_queue == NULL) {
        return false;
    }
    
    TickType_t timeout_ticks = (timeout_ms == UINT32_MAX) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xQueueReceive(rotary_ctx.event_queue, event, timeout_ticks) == pdTRUE;
}

esp_err_t rotary_start(void) {
    if (!rotary_ctx.initialized) {
        ESP_LOGE(TAG, "Rotary encoder not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (rotary_ctx.running) {
        ESP_LOGW(TAG, "Rotary encoder already running");
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Starting rotary encoder task");
    
    BaseType_t result = xTaskCreate(
        rotary_task,
        "rotary_task",
        2048,
        NULL,
        5,
        &rotary_ctx.task_handle
    );
    
    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create rotary task");
        return ESP_ERR_NO_MEM;
    }
    
    rotary_ctx.running = true;
    return ESP_OK;
}

void rotary_stop(void) {
    if (!rotary_ctx.running) {
        return;
    }
    
    ESP_LOGI(TAG, "Stopping rotary encoder task");
    
    rotary_ctx.running = false;
    
    if (rotary_ctx.task_handle != NULL) {
        vTaskDelete(rotary_ctx.task_handle);
        rotary_ctx.task_handle = NULL;
    }
}

static uint8_t rotary_read_pins(void) {
    uint8_t state = 0;
    if (gpio_get_level(rotary_ctx.config.pin_a)) state |= 0x01;
    if (gpio_get_level(rotary_ctx.config.pin_b)) state |= 0x02;
    return state;
}

static void IRAM_ATTR rotary_isr_handler(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    uint32_t current_time = xTaskGetTickCountFromISR();
    uint8_t current_state = rotary_read_pins();
    
    // Debounce check
    if ((current_time - rotary_ctx.last_change_time) < pdMS_TO_TICKS(rotary_ctx.config.debounce_ms)) {
        return;
    }
    
    // Determine rotation direction using state machine
    uint8_t combined_state = (rotary_ctx.last_state << 2) | current_state;
    rotary_event_t event = ROTARY_EVENT_NONE;
    
    // Gray code sequence for quadrature encoder
    switch (combined_state) {
        case 0x01: // 00 -> 01
        case 0x07: // 01 -> 11
        case 0x08: // 10 -> 00
        case 0x0E: // 11 -> 10
            event = rotary_ctx.config.invert_direction ? ROTARY_EVENT_CCW : ROTARY_EVENT_CW;
            break;
        case 0x02: // 00 -> 10
        case 0x0B: // 10 -> 11
        case 0x0D: // 11 -> 01
        case 0x04: // 01 -> 00
            event = rotary_ctx.config.invert_direction ? ROTARY_EVENT_CW : ROTARY_EVENT_CCW;
            break;
        default:
            // Invalid transition or no change
            break;
    }
    
    if (event != ROTARY_EVENT_NONE) {
        // Send event to queue
        xQueueSendFromISR(rotary_ctx.event_queue, &event, &xHigherPriorityTaskWoken);
        
        // Call callback if set
        if (rotary_ctx.callback != NULL) {
            rotary_ctx.callback(event);
        }
    }
    
    rotary_ctx.last_state = current_state;
    rotary_ctx.last_change_time = current_time;
    
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

static void IRAM_ATTR button_isr_handler(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    uint32_t current_time = xTaskGetTickCountFromISR();
    
    // Simple debounce for button
    if ((current_time - rotary_ctx.button_press_time) < pdMS_TO_TICKS(50)) {
        return;
    }
    
    rotary_event_t event = ROTARY_EVENT_BUTTON;
    xQueueSendFromISR(rotary_ctx.event_queue, &event, &xHigherPriorityTaskWoken);
    
    // Call callback if set
    if (rotary_ctx.callback != NULL) {
        rotary_ctx.callback(event);
    }
    
    rotary_ctx.button_press_time = current_time;
    
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

static void rotary_task(void *pvParameters) {
    ESP_LOGI(TAG, "Rotary encoder task started");
    
    rotary_event_t event;
    
    while (rotary_ctx.running) {
        // Wait for events and process them
        if (rotary_wait_event(&event, 100)) {
            ESP_LOGD(TAG, "Rotary event: %d", event);
            
            // Events are already handled in ISR and callback
            // This task can be used for additional processing if needed
        }
    }
    
    ESP_LOGI(TAG, "Rotary encoder task stopped");
    vTaskDelete(NULL);
}
