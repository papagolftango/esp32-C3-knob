/**
 * @file lv_conf.h
 * LVGL Configuration for ESP-IDF
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0

/*====================
   MEMORY SETTINGS  
 *====================*/
#define LV_MEM_CUSTOM 0
#if LV_MEM_CUSTOM == 0
    #define LV_MEM_SIZE (64U * 1024U)
#endif

/*====================
   HAL SETTINGS
 *====================*/
#define LV_DISP_DEF_REFR_PERIOD 30
#define LV_INDEV_DEF_READ_PERIOD 30
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "freertos/FreeRTOS.h"
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (xTaskGetTickCount() * portTICK_PERIOD_MS)
#endif

/*====================
   FEATURE CONFIGURATION
 *====================*/
#define LV_DRAW_COMPLEX 1
#define LV_SHADOW_CACHE_SIZE 0
#define LV_CIRCLE_CACHE_SIZE 4
#define LV_IMG_CACHE_DEF_SIZE 0
#define LV_GRADIENT_MAX_STOPS 2
#define LV_GRAD_CACHE_DEF_SIZE 0
#define LV_DITHER_GRADIENT 0

/*====================
   LOGGING
 *====================*/
#define LV_USE_LOG 1
#if LV_USE_LOG
    #define LV_LOG_LEVEL LV_LOG_LEVEL_INFO
    #define LV_LOG_PRINTF 1
#endif

/*====================
   ASSERTS
 *====================*/
#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0

/*====================
   FONT USAGE
 *====================*/
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 0

#define LV_FONT_DEFAULT &lv_font_montserrat_14

/*====================
   TEXT SETTINGS
 *====================*/
#define LV_TXT_ENC LV_TXT_ENC_UTF8
#define LV_TXT_BREAK_CHARS " ,.;:-_"

/*====================
   WIDGET USAGE
 *====================*/
#define LV_USE_ARC        1
#define LV_USE_BAR        1
#define LV_USE_BTN        1
#define LV_USE_BTNMATRIX  1
#define LV_USE_CANVAS     0
#define LV_USE_CHECKBOX   1
#define LV_USE_DROPDOWN   1
#define LV_USE_IMG        1
#define LV_USE_LABEL      1
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION 1
    #define LV_LABEL_LONG_TXT_HINT 1
#endif
#define LV_USE_LINE       1
#define LV_USE_ROLLER     1
#define LV_USE_SLIDER     1
#define LV_USE_SWITCH     1
#define LV_USE_TEXTAREA   1
#define LV_USE_TABLE      1

/*====================
   THEMES
 *====================*/
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    #define LV_THEME_DEFAULT_DARK 0
    #define LV_THEME_DEFAULT_GROW 1
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif

#define LV_USE_THEME_BASIC 1

/*====================
   LAYOUTS
 *====================*/
#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*====================
   3RD PARTY LIBRARIES
 *====================*/
#define LV_USE_FS_STDIO 0
#define LV_USE_FS_POSIX 0
#define LV_USE_FS_WIN32 0
#define LV_USE_FS_FATFS 0
#define LV_USE_PNG 0
#define LV_USE_BMP 0
#define LV_USE_SJPG 0
#define LV_USE_GIF 0
#define LV_USE_QRCODE 0
#define LV_USE_FREETYPE 0

/*====================
   OTHERS
 *====================*/
#define LV_USE_SNAPSHOT 0
#define LV_USE_MONKEY 0
#define LV_USE_GRIDNAV 0
#define LV_USE_FRAGMENT 0
#define LV_USE_IMGFONT 0
#define LV_USE_MSG 0

/*====================
   EXAMPLES
 *====================*/
#define LV_BUILD_EXAMPLES 0

#endif /*LV_CONF_H*/
