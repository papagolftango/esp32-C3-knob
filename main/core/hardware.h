#pragma once

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <driver/i2c_master.h>

// Hardware configuration for ESP32-S3-Knob-Touch-LCD-1.8
#define ENCODER_A_PIN   GPIO_NUM_8
#define ENCODER_B_PIN   GPIO_NUM_7
#define ENCODER_BTN_PIN GPIO_NUM_9   // Rotary encoder button (set to GPIO_NUM_NC if no button)

// If your hardware doesn't have a push button, uncomment this line:
// #define ENCODER_BTN_PIN GPIO_NUM_NC
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize all hardware peripherals
 */
void hardware_init(void);

/**
 * @brief Initialize rotary encoder with default configuration
 */
void hardware_init_rotary(void);

/**
 * @brief Initialize SPI for display
 */
void hardware_init_spi(void);

/**
 * @brief Initialize I2C for haptic feedback
 */
void hardware_init_i2c(void);

#ifdef __cplusplus
}
#endif
