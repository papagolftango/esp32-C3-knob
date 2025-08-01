#include "settings_ui.h"

// Static member definitions
int SettingsUI::selected_item = 0;
bool SettingsUI::menu_active = false;

void SettingsUI::begin() {
    selected_item = 0;
    menu_active = false;
}

void SettingsUI::updateScreen() {
    createScreen();
}

void SettingsUI::handleEncoderRotation(int direction) {
    if (menu_active) {
        if (direction > 0) {
            selected_item = (selected_item + 1) % MENU_ITEMS;
        } else {
            selected_item = (selected_item - 1 + MENU_ITEMS) % MENU_ITEMS;
        }
        HapticFeedback::menuNavigate();
        Serial.printf("Settings menu: item %d selected\n", selected_item);
    }
}

void SettingsUI::handleSelection() {
    if (!menu_active) {
        // Activate settings menu navigation
        setMenuActive(true);
        HapticFeedback::confirmation();
        Serial.println("Settings menu activated");
    } else {
        // Touch selects/toggles current menu item
        switch (selected_item) {
            case 0: // Haptic Feedback toggle
                handleHapticToggle();
                break;
            case 1: // WiFi Reset
                handleWiFiReset();
                break;
        }
    }
}

bool SettingsUI::isMenuActive() {
    return menu_active;
}

void SettingsUI::setMenuActive(bool active) {
    menu_active = active;
}

void SettingsUI::reset() {
    selected_item = 0;
    menu_active = false;
}

void SettingsUI::createScreen() {
    // Create title
    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "⚙️ SETTINGS");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
    
    // Settings menu with selection indicators
    lv_obj_t *menu_label = lv_label_create(lv_scr_act());
    String settings_text = "";
    
    // Haptic Feedback setting (item 0)
    settings_text += (selected_item == 0) ? "▶ " : "  ";
    settings_text += "Haptic Feedback: ";
    settings_text += HapticFeedback::isEnabled() ? "ON 🟢" : "OFF 🔴";
    settings_text += "\n";
    
    // WiFi Reset setting (item 1)
    settings_text += (selected_item == 1) ? "▶ " : "  ";
    settings_text += "Reset WiFi\n";
    
    // Non-interactive items
    settings_text += "  Brightness: 80%\n";
    settings_text += "  Version: 1.0.0\n";
    settings_text += "  Board: ESP32-S3";
    
    lv_label_set_text(menu_label, settings_text.c_str());
    lv_obj_set_style_text_font(menu_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(menu_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_center(menu_label);
    
    // Instructions
    lv_obj_t *instructions = lv_label_create(lv_scr_act());
    String inst_text = "Rotate: Navigate  Touch: Select/Toggle";
    if (HapticFeedback::isEnabled()) {
        inst_text += "\n🎛️ Haptic feedback enabled";
    }
    lv_label_set_text(instructions, inst_text.c_str());
    lv_obj_align(instructions, LV_ALIGN_BOTTOM_MID, 0, -10);
}

void SettingsUI::handleHapticToggle() {
    HapticFeedback::setEnabled(!HapticFeedback::isEnabled());
    HapticFeedback::confirmation();
    Serial.printf("Haptic feedback %s\n", HapticFeedback::isEnabled() ? "enabled" : "disabled");
}

void SettingsUI::handleWiFiReset() {
    Serial.println("WiFi Reset selected (not implemented)");
    HapticFeedback::error();
}
    settings_text += "  Factory Reset\\n";
    settings_text += "  Display Info";
    
    return settings_text;
}

String SettingsUI::buildInstructionsText(bool mock_enabled, bool haptic_enabled) {
    String inst_text = "Rotate: Navigate  Touch: Select/Toggle";
    
    if (mock_enabled) {
        inst_text += "\\n\\n🎯 DEMO MODE ACTIVE";
    }
    if (haptic_enabled) {
        inst_text += "\\n🎛️ Haptic feedback enabled";
    }
    
    return inst_text;
}
