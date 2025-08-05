#pragma once

#include "lvgl.h"

// Display configuration
#define DISPLAY_WIDTH   360
#define DISPLAY_HEIGHT  360
#define DISPLAY_BUF_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT / 10)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize LVGL and display driver
 */
void display_init(void);

/**
 * @brief Start LVGL task for handling UI updates
 */
void display_start_task(void);

/**
 * @brief LVGL display flush callback
 */
void display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

// TODO: Add touch support when hardware arrives
// void touch_init(void);
// void touch_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

#ifdef __cplusplus
}
#endif
