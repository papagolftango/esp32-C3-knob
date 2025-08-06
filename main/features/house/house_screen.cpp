#include "house_screen.h"
#include "bin_collection_manager.h"
#include "motd_manager.h"
#include "../core/mqtt_manager.h"
#include "esp_log.h"

static const char* TAG = "house_screen";
static lv_obj_t* house_screen = NULL;
static lv_obj_t* bin_collection_label = NULL;
static lv_obj_t* bin_icons_container = NULL;
static lv_obj_t* black_bin_icon = NULL;
static lv_obj_t* green_bin_icon = NULL;
static lv_obj_t* orange_bin_icon = NULL;
static lv_obj_t* lorry_icon = NULL;
static lv_timer_t* flash_timer = NULL;

// MOTD ticker variables
static lv_obj_t* motd_label = NULL;
static lv_timer_t* scroll_timer = NULL;
static int scroll_position = 0;
static std::string current_motd_text = "";

static void flash_timer_cb(lv_timer_t * timer) {
    // Flash bins that are in PREPARE_TONIGHT state
    BinCollectionData black_data = BinCollectionManager::getBlackBinData();
    BinCollectionData green_data = BinCollectionManager::getGreenBinData();
    BinCollectionData orange_data = BinCollectionManager::getOrangeBinData();
    
    static bool flash_visible = true;
    flash_visible = !flash_visible;
    
    // Flash black bin if preparing tonight
    if (black_data.status == BinCollectionStatus::PREPARE_TONIGHT && black_bin_icon) {
        lv_obj_set_style_opa(black_bin_icon, flash_visible ? LV_OPA_100 : LV_OPA_30, 0);
    }
    
    // Flash green bin if preparing tonight
    if (green_data.status == BinCollectionStatus::PREPARE_TONIGHT && green_bin_icon) {
        lv_obj_set_style_opa(green_bin_icon, flash_visible ? LV_OPA_100 : LV_OPA_30, 0);
    }
    
    // Flash orange bin if preparing tonight
    if (orange_data.status == BinCollectionStatus::PREPARE_TONIGHT && orange_bin_icon) {
        lv_obj_set_style_opa(orange_bin_icon, flash_visible ? LV_OPA_100 : LV_OPA_30, 0);
    }
}

static void scroll_timer_cb(lv_timer_t * timer) {
    if (!motd_label || current_motd_text.empty()) return;
    
    // Get the width of the label container and text
    lv_coord_t container_width = lv_obj_get_width(motd_label);
    lv_coord_t text_width = lv_txt_get_width(current_motd_text.c_str(), 
                                           current_motd_text.length(), 
                                           lv_obj_get_style_text_font(motd_label, LV_PART_MAIN), 
                                           0, LV_TEXT_FLAG_NONE);
    
    // Only scroll if text is wider than container
    if (text_width > container_width) {
        scroll_position += 2; // Scroll speed (pixels per timer tick)
        
        // Reset position when text has completely scrolled past
        if (scroll_position > text_width + container_width) {
            scroll_position = -container_width;
        }
        
        // Update the label position
        lv_obj_set_x(motd_label, -scroll_position);
    } else {
        // Center the text if it fits
        lv_obj_align(motd_label, LV_ALIGN_CENTER, 0, 0);
    }
}

static void updateMOTDDisplay(void) {
    if (!motd_label) return;
    
    // Check if MOTD has changed
    if (MOTDManager::hasMessageChanged()) {
        current_motd_text = MOTDManager::getCurrentMessage();
        lv_label_set_text(motd_label, current_motd_text.c_str());
        scroll_position = 0; // Reset scroll position
        MOTDManager::clearChangeFlag();
        ESP_LOGI(TAG, "MOTD updated: %s", current_motd_text.c_str());
    }
}

static void handleMOTDMessage(const char* topic, const char* message) {
    ESP_LOGI(TAG, "MOTD message received on %s: %s", topic, message);
    
    // Check if this is the MOTD topic
    if (strstr(topic, "home/motd") != nullptr) {
        MOTDManager::updateMessage(message);
    }
}

static void updateBinCollectionDisplay(void) {
    if (!bin_icons_container) return;
    
    // Check if bin collection data has changed
    BinCollectionManager::hasDataChanged();
    
    // Get current bin states
    BinCollectionData black_data = BinCollectionManager::getBlackBinData();
    BinCollectionData green_data = BinCollectionManager::getGreenBinData();
    BinCollectionData orange_data = BinCollectionManager::getOrangeBinData();
    
    // Hide all icons initially
    if (black_bin_icon) lv_obj_add_flag(black_bin_icon, LV_OBJ_FLAG_HIDDEN);
    if (green_bin_icon) lv_obj_add_flag(green_bin_icon, LV_OBJ_FLAG_HIDDEN);
    if (orange_bin_icon) lv_obj_add_flag(orange_bin_icon, LV_OBJ_FLAG_HIDDEN);
    if (lorry_icon) lv_obj_add_flag(lorry_icon, LV_OBJ_FLAG_HIDDEN);
    
    // Reset opacity for all bins (stop flashing)
    if (black_bin_icon) lv_obj_set_style_opa(black_bin_icon, LV_OPA_100, 0);
    if (green_bin_icon) lv_obj_set_style_opa(green_bin_icon, LV_OPA_100, 0);
    if (orange_bin_icon) lv_obj_set_style_opa(orange_bin_icon, LV_OPA_100, 0);
    
    bool show_flash_timer = false;
    bool show_lorry = false;
    
    // Show black bin if relevant
    if (black_data.status == BinCollectionStatus::DUE_TODAY ||
        black_data.status == BinCollectionStatus::PREPARE_TONIGHT ||
        black_data.status == BinCollectionStatus::NEXT_COLLECTION) {
        if (black_bin_icon) {
            lv_obj_clear_flag(black_bin_icon, LV_OBJ_FLAG_HIDDEN);
            if (black_data.status == BinCollectionStatus::DUE_TODAY) {
                show_lorry = true;
            }
            if (black_data.status == BinCollectionStatus::PREPARE_TONIGHT) {
                show_flash_timer = true;
            }
        }
    }
    
    // Show green bin if relevant
    if (green_data.status == BinCollectionStatus::DUE_TODAY ||
        green_data.status == BinCollectionStatus::PREPARE_TONIGHT ||
        green_data.status == BinCollectionStatus::NEXT_COLLECTION) {
        if (green_bin_icon) {
            lv_obj_clear_flag(green_bin_icon, LV_OBJ_FLAG_HIDDEN);
            if (green_data.status == BinCollectionStatus::DUE_TODAY) {
                show_lorry = true;
            }
            if (green_data.status == BinCollectionStatus::PREPARE_TONIGHT) {
                show_flash_timer = true;
            }
        }
    }
    
    // Show orange bin if relevant
    if (orange_data.status == BinCollectionStatus::DUE_TODAY ||
        orange_data.status == BinCollectionStatus::PREPARE_TONIGHT ||
        orange_data.status == BinCollectionStatus::NEXT_COLLECTION) {
        if (orange_bin_icon) {
            lv_obj_clear_flag(orange_bin_icon, LV_OBJ_FLAG_HIDDEN);
            if (orange_data.status == BinCollectionStatus::DUE_TODAY) {
                show_lorry = true;
            }
            if (orange_data.status == BinCollectionStatus::PREPARE_TONIGHT) {
                show_flash_timer = true;
            }
        }
    }
    
    // Show lorry if collection day
    if (show_lorry && lorry_icon) {
        lv_obj_clear_flag(lorry_icon, LV_OBJ_FLAG_HIDDEN);
    }
    
    // Start/stop flash timer
    if (show_flash_timer) {
        if (!flash_timer) {
            flash_timer = lv_timer_create(flash_timer_cb, 500, NULL); // Flash every 500ms
        }
    } else {
        if (flash_timer) {
            lv_timer_del(flash_timer);
            flash_timer = NULL;
        }
    }
    
    // Update text status
    std::string display_text = BinCollectionManager::getDisplayStatus();
    if (bin_collection_label) {
        lv_label_set_text(bin_collection_label, display_text.c_str());
    }
    
    // Update MOTD display
    updateMOTDDisplay();
    
    ESP_LOGI(TAG, "Bin collection display updated: %s", display_text.c_str());
}

void house_screen_init(void) {
    // Create screen
    house_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(house_screen, lv_color_hex(0x2F1B14), 0);
    
    // Title
    lv_obj_t* title = lv_label_create(house_screen);
    lv_label_set_text(title, "🏠 BIN COLLECTION");
    lv_obj_set_style_text_color(title, lv_color_hex(0xDEB887), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Container for bin icons
    bin_icons_container = lv_obj_create(house_screen);
    lv_obj_set_size(bin_icons_container, 200, 60);
    lv_obj_set_style_bg_opa(bin_icons_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(bin_icons_container, LV_OPA_TRANSP, 0);
    lv_obj_align(bin_icons_container, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_flex_flow(bin_icons_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bin_icons_container, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Black bin icon
    black_bin_icon = lv_label_create(bin_icons_container);
    lv_label_set_text(black_bin_icon, "🗑️");
    lv_obj_set_style_text_color(black_bin_icon, lv_color_hex(0x404040), 0); // Dark gray/black
    lv_obj_set_style_text_font(black_bin_icon, &lv_font_montserrat_18, 0);
    lv_obj_add_flag(black_bin_icon, LV_OBJ_FLAG_HIDDEN);
    
    // Green bin icon
    green_bin_icon = lv_label_create(bin_icons_container);
    lv_label_set_text(green_bin_icon, "🗑️");
    lv_obj_set_style_text_color(green_bin_icon, lv_color_hex(0x228B22), 0); // Forest green
    lv_obj_set_style_text_font(green_bin_icon, &lv_font_montserrat_18, 0);
    lv_obj_add_flag(green_bin_icon, LV_OBJ_FLAG_HIDDEN);
    
    // Orange bin icon
    orange_bin_icon = lv_label_create(bin_icons_container);
    lv_label_set_text(orange_bin_icon, "🗑️");
    lv_obj_set_style_text_color(orange_bin_icon, lv_color_hex(0xFF8C00), 0); // Dark orange
    lv_obj_set_style_text_font(orange_bin_icon, &lv_font_montserrat_18, 0);
    lv_obj_add_flag(orange_bin_icon, LV_OBJ_FLAG_HIDDEN);
    
    // Lorry icon
    lorry_icon = lv_label_create(bin_icons_container);
    lv_label_set_text(lorry_icon, "🚛");
    lv_obj_set_style_text_color(lorry_icon, lv_color_hex(0xFFD700), 0); // Gold
    lv_obj_set_style_text_font(lorry_icon, &lv_font_montserrat_18, 0);
    lv_obj_add_flag(lorry_icon, LV_OBJ_FLAG_HIDDEN);
    
    // Status text
    bin_collection_label = lv_label_create(house_screen);
    lv_label_set_text(bin_collection_label, "Loading schedule...");
    lv_obj_set_style_text_color(bin_collection_label, lv_color_hex(0x87CEEB), 0);
    lv_obj_align(bin_collection_label, LV_ALIGN_CENTER, 0, 40);
    
    // MOTD ticker container at bottom
    lv_obj_t* motd_container = lv_obj_create(house_screen);
    lv_obj_set_size(motd_container, LV_HOR_RES - 20, 30);
    lv_obj_set_style_bg_color(motd_container, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_border_color(motd_container, lv_color_hex(0x444444), 0);
    lv_obj_set_style_border_width(motd_container, 1, 0);
    lv_obj_set_style_radius(motd_container, 5, 0);
    lv_obj_align(motd_container, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_clip_corner(motd_container, true, 0);
    
    // MOTD ticker label
    motd_label = lv_label_create(motd_container);
    lv_obj_set_style_text_color(motd_label, lv_color_hex(0x00FF00), 0); // Green ticker text
    lv_obj_set_style_text_font(motd_label, &lv_font_montserrat_14, 0);
    lv_obj_align(motd_label, LV_ALIGN_LEFT_MID, 5, 0);
    
    // Initialize MOTD manager and set initial text
    MOTDManager::init();
    current_motd_text = MOTDManager::getCurrentMessage();
    lv_label_set_text(motd_label, current_motd_text.c_str());
    
    // Start scroll timer for MOTD (different rate from flash timer)
    scroll_timer = lv_timer_create(scroll_timer_cb, 80, NULL); // Scroll every 80ms
    
    // Register MQTT handler for MOTD messages
    MQTTManager::registerHandler("home", handleMOTDMessage);
    
    ESP_LOGI(TAG, "Bin collection screen initialized with icons and MOTD ticker");
}

void house_screen_show(void) {
    if (house_screen) {
        lv_scr_load(house_screen);
        ESP_LOGI(TAG, "Bin collection screen shown");
        
        // Update bin collection information
        updateBinCollectionDisplay();
    }
}

void house_screen_handle_button_press(void) {
    ESP_LOGI(TAG, "Bin collection button pressed");
    
    // For now, just refresh the display
    // In future, could be used for manual schedule updates or other actions
    updateBinCollectionDisplay();
}
