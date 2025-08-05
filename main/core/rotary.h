#pragma once

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

// Rotary encoder events
typedef enum {
    ROTARY_EVENT_NONE = 0,
    ROTARY_EVENT_CW,        // Clockwise rotation
    ROTARY_EVENT_CCW,       // Counter-clockwise rotation
    ROTARY_EVENT_BUTTON     // Button press
} rotary_event_t;

// Rotary encoder configuration
typedef struct {
    gpio_num_t pin_a;       // Encoder A pin
    gpio_num_t pin_b;       // Encoder B pin
    gpio_num_t pin_btn;     // Button pin (if available)
    bool invert_direction;  // Invert rotation direction
    uint32_t debounce_ms;   // Debounce time in milliseconds
} rotary_config_t;

// Callback function type for rotary events
typedef void (*rotary_event_callback_t)(rotary_event_t event);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize rotary encoder
 * @param config Rotary encoder configuration
 * @return ESP_OK on success
 */
esp_err_t rotary_init(const rotary_config_t *config);

/**
 * @brief Set callback function for rotary events
 * @param callback Callback function to handle rotary events
 */
void rotary_set_callback(rotary_event_callback_t callback);

/**
 * @brief Get the next rotary event from queue (non-blocking)
 * @param event Pointer to store the event
 * @return true if event was retrieved, false if queue is empty
 */
bool rotary_get_event(rotary_event_t *event);

/**
 * @brief Wait for next rotary event (blocking)
 * @param event Pointer to store the event
 * @param timeout_ms Timeout in milliseconds (portMAX_DELAY for infinite)
 * @return true if event was retrieved, false on timeout
 */
bool rotary_wait_event(rotary_event_t *event, uint32_t timeout_ms);

/**
 * @brief Start rotary encoder task
 * @return ESP_OK on success
 */
esp_err_t rotary_start(void);

/**
 * @brief Stop rotary encoder task
 */
void rotary_stop(void);

#ifdef __cplusplus
}
#endif
