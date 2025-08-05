#pragma once

#include "core/hardware.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Test if button hardware is connected and working
 * 
 * This function will:
 * - Configure GPIO_9 as input with pullup
 * - Monitor for button state changes for 10 seconds
 * - Log button presses and releases
 * - Report if button hardware is detected
 */
void test_button_hardware(void);

#ifdef __cplusplus
}
#endif
