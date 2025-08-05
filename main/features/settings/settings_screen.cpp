#include "settings_screen.h"
#include <esp_log.h>

// Include device control for MQTT integration
#include "../device_control/device_control_manager.h"

static const char *TAG = "SETTINGS_SCREEN";

// Forward declarations
static void update_selection_highlight(void);

static lv_obj_t *settings_screen;
static lv_obj_t *title_label;
static lv_obj_t *settings_list;

// Navigation state
static int current_selection = 0;
static int total_items = 4;
static lv_obj_t *setting_buttons[4];  // Array to hold button references

void settings_screen_init(void) {
    ESP_LOGI(TAG, "Initializing settings screen");
    
    // Create screen object
    settings_screen = lv_obj_create(NULL);
    
    // Create title label
    title_label = lv_label_create(settings_screen);
    lv_label_set_text(title_label, "Settings");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 20);
    
    // Create settings list
    settings_list = lv_list_create(settings_screen);
    lv_obj_set_size(settings_list, 280, 200);
    lv_obj_align(settings_list, LV_ALIGN_CENTER, 0, 20);
    
    // Add some example settings items and store references
    setting_buttons[0] = lv_list_add_btn(settings_list, LV_SYMBOL_WIFI, "WiFi Reset");
    setting_buttons[1] = lv_list_add_btn(settings_list, LV_SYMBOL_SETTINGS, "Display Brightness");
    setting_buttons[2] = lv_list_add_btn(settings_list, LV_SYMBOL_VOLUME_MAX, "Haptic Feedback");
    setting_buttons[3] = lv_list_add_btn(settings_list, LV_SYMBOL_REFRESH, "Factory Reset");
    
    // Set initial selection (highlight first item)
    update_selection_highlight();
    
    // Add navigation hint
    lv_obj_t *nav_label = lv_label_create(settings_screen);
    lv_label_set_text(nav_label, "Rotate: Navigate | Press: Select");
    lv_obj_set_style_text_font(nav_label, &lv_font_montserrat_14, 0);
    lv_obj_align(nav_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    ESP_LOGI(TAG, "Settings screen initialized");
}

// Helper function to update selection highlight
static void update_selection_highlight(void) {
    // Reset all buttons to normal state
    for (int i = 0; i < total_items; i++) {
        if (setting_buttons[i]) {
            lv_obj_set_style_bg_color(setting_buttons[i], lv_palette_main(LV_PALETTE_GREY), 0);
            lv_obj_set_style_bg_opa(setting_buttons[i], LV_OPA_20, 0);
        }
    }
    
    // Highlight current selection
    if (setting_buttons[current_selection]) {
        lv_obj_set_style_bg_color(setting_buttons[current_selection], lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_set_style_bg_opa(setting_buttons[current_selection], LV_OPA_50, 0);
    }
    
    ESP_LOGI(TAG, "Selection: %d/%d", current_selection + 1, total_items);
}


void settings_screen_show(void) {
    ESP_LOGI(TAG, "Showing settings screen");
    lv_scr_load(settings_screen);
}

void settings_screen_handle_rotation(int direction) {
    ESP_LOGI(TAG, "Rotary rotation: %d", direction);
    
    // Update selection based on rotation
    current_selection += direction;
    
    // Wrap around at boundaries
    if (current_selection < 0) {
        current_selection = total_items - 1;
    } else if (current_selection >= total_items) {
        current_selection = 0;
    }
    
    // Update visual highlight
    update_selection_highlight();
}

void settings_screen_handle_button_press(void) {
    ESP_LOGI(TAG, "Button pressed on settings screen - item %d selected", current_selection);
    
    // Handle the selected setting
    switch (current_selection) {
        case 0: // WiFi Reset
            ESP_LOGI(TAG, "WiFi Reset selected");
            // Trigger WiFi credentials reset
            // This will be processed by DeviceControlManager when implemented
            ESP_LOGI(TAG, "Triggering WiFi reset...");
            // TODO: Call WiFiManager reset function
            break;
            
        case 1: // Display Brightness
            ESP_LOGI(TAG, "Display Brightness selected");
            // Cycle through brightness levels: 25%, 50%, 75%, 100%
            settings_screen_cycle_brightness();
            break;
            
        case 2: // Haptic Feedback
            ESP_LOGI(TAG, "Haptic Feedback selected");
            // Toggle haptic feedback via DeviceControlManager
            settings_screen_toggle_haptic();
            break;
            
        case 3: // Factory Reset
            ESP_LOGI(TAG, "Factory Reset selected");
            // TODO: Show confirmation dialog first
            ESP_LOGI(TAG, "Factory reset requested - implement confirmation dialog");
            break;
            
        default:
            ESP_LOGW(TAG, "Unknown selection: %d", current_selection);
            break;
    }
}

// Helper function to cycle through brightness levels
void settings_screen_cycle_brightness(void) {
    int current = DeviceControlManager::getCurrentBrightness();
    int new_brightness;
    
    // Cycle: 25% -> 50% -> 75% -> 100% -> 25%
    if (current <= 25) {
        new_brightness = 50;
    } else if (current <= 50) {
        new_brightness = 75;
    } else if (current <= 75) {
        new_brightness = 100;
    } else {
        new_brightness = 25;
    }
    
    DeviceControlManager::setBrightness(new_brightness);
    ESP_LOGI(TAG, "Brightness cycled to %d%%", new_brightness);
}

// Helper function to toggle haptic feedback
void settings_screen_toggle_haptic(void) {
    bool current = DeviceControlManager::isHapticEnabled();
    bool new_state = !current;
    
    DeviceControlManager::setHapticEnabled(new_state);
    ESP_LOGI(TAG, "Haptic feedback %s", new_state ? "enabled" : "disabled");
}
