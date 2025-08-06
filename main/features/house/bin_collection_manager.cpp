#include "bin_collection_manager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

static const char *TAG = "BIN_COLLECTION";

// Static member definitions
BinCollectionData BinCollectionManager::black_bin = {BinType::BLACK, BinCollectionStatus::UNKNOWN, 0, "Unknown"};
BinCollectionData BinCollectionManager::orange_bin = {BinType::ORANGE, BinCollectionStatus::UNKNOWN, 0, "Unknown"};
BinCollectionData BinCollectionManager::green_bin = {BinType::GREEN, BinCollectionStatus::UNKNOWN, 0, "Unknown"};

// State tracking
bool BinCollectionManager::data_changed = false;
unsigned long BinCollectionManager::last_update = 0;

void BinCollectionManager::begin() {
    ESP_LOGI(TAG, "Initializing Bin Collection Manager");
    
    // Initialize with default state - no collections scheduled yet
    resetAllStatus();
    data_changed = true;
    
    ESP_LOGI(TAG, "Bin Collection Manager initialized - waiting for schedule data");
}

bool BinCollectionManager::hasDataChanged() {
    if (data_changed) {
        data_changed = false;
        return true;
    }
    return false;
}

bool BinCollectionManager::isDataValid() {
    // Data is valid if we have schedule information
    return (black_bin.status != BinCollectionStatus::UNKNOWN) ||
           (orange_bin.status != BinCollectionStatus::UNKNOWN) ||
           (green_bin.status != BinCollectionStatus::UNKNOWN);
}

BinType BinCollectionManager::getTodaysCollection() {
    if (black_bin.status == BinCollectionStatus::DUE_TODAY) {
        return BinType::BLACK;
    }
    if (green_bin.status == BinCollectionStatus::DUE_TODAY) {
        return BinType::GREEN; // Represents Green+Orange day
    }
    return BinType::NONE;
}

std::string BinCollectionManager::getDisplayStatus() {
    // Priority: DUE_TODAY > PREPARE_TONIGHT > NEXT_COLLECTION
    
    if (black_bin.status == BinCollectionStatus::DUE_TODAY) {
        return "🗑️ Black bin DUE TODAY!";
    }
    if (green_bin.status == BinCollectionStatus::DUE_TODAY || orange_bin.status == BinCollectionStatus::DUE_TODAY) {
        return "🗑️ Green+Orange DUE TODAY!";
    }
    
    if (black_bin.status == BinCollectionStatus::PREPARE_TONIGHT) {
        return "🗑️ Prepare Black bin tonight";
    }
    if (green_bin.status == BinCollectionStatus::PREPARE_TONIGHT || orange_bin.status == BinCollectionStatus::PREPARE_TONIGHT) {
        return "🗑️ Prepare Green+Orange tonight";
    }
    
    // Show next collection
    if (black_bin.days_until_next > 0) {
        return "🗑️ Black bin in " + std::to_string(black_bin.days_until_next) + " days";
    }
    if (green_bin.days_until_next > 0) {
        return "🗑️ Green+Orange in " + std::to_string(green_bin.days_until_next) + " days";
    }
    
    return "🗑️ No schedule data";
}

void BinCollectionManager::updateSchedule(BinType tomorrow_bin, BinType today_bin, int days_to_next) {
    ESP_LOGI(TAG, "Updating schedule - Today: %s, Tomorrow: %s, Days to next: %d",
             binTypeToString(today_bin).c_str(), binTypeToString(tomorrow_bin).c_str(), days_to_next);
    
    // Reset all bins first
    resetAllStatus();
    
    // Set today's collection if any
    if (today_bin == BinType::BLACK) {
        black_bin.status = BinCollectionStatus::DUE_TODAY;
    } else if (today_bin == BinType::GREEN) {
        green_bin.status = BinCollectionStatus::DUE_TODAY;
        orange_bin.status = BinCollectionStatus::DUE_TODAY;
    }
    
    // Set tomorrow's preparation if any
    if (tomorrow_bin == BinType::BLACK) {
        black_bin.status = BinCollectionStatus::PREPARE_TONIGHT;
        black_bin.days_until_next = 1;
    } else if (tomorrow_bin == BinType::GREEN) {
        green_bin.status = BinCollectionStatus::PREPARE_TONIGHT;
        orange_bin.status = BinCollectionStatus::PREPARE_TONIGHT;
        green_bin.days_until_next = 1;
        orange_bin.days_until_next = 1;
    }
    
    // Set next collection countdown for bins not active today/tomorrow
    if (today_bin == BinType::NONE && tomorrow_bin == BinType::NONE) {
        // No immediate collections, set next collection info
        if (days_to_next > 1) {
            // Determine which bins are next (this would come from schedule data)
            // For now, assume alternating weeks
            black_bin.status = BinCollectionStatus::NEXT_COLLECTION;
            black_bin.days_until_next = days_to_next;
        }
    }
    
    data_changed = true;
    last_update = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    ESP_LOGI(TAG, "Schedule updated successfully");
}

std::string BinCollectionManager::binTypeToString(BinType type) {
    switch (type) {
        case BinType::BLACK: return "Black";
        case BinType::ORANGE: return "Orange";
        case BinType::GREEN: return "Green";
        case BinType::NONE: return "None";
        default: return "Unknown";
    }
}

std::string BinCollectionManager::statusToString(BinCollectionStatus status) {
    switch (status) {
        case BinCollectionStatus::PREPARE_TONIGHT: return "Prepare Tonight";
        case BinCollectionStatus::DUE_TODAY: return "Due Today";
        case BinCollectionStatus::NEXT_COLLECTION: return "Next Collection";
        case BinCollectionStatus::UNKNOWN: return "Unknown";
        default: return "Invalid";
    }
}

void BinCollectionManager::resetAllStatus() {
    black_bin.status = BinCollectionStatus::UNKNOWN;
    black_bin.days_until_next = 0;
    black_bin.next_collection_day = "Unknown";
    
    orange_bin.status = BinCollectionStatus::UNKNOWN;
    orange_bin.days_until_next = 0;
    orange_bin.next_collection_day = "Unknown";
    
    green_bin.status = BinCollectionStatus::UNKNOWN;
    green_bin.days_until_next = 0;
    green_bin.next_collection_day = "Unknown";
    
    ESP_LOGI(TAG, "All bin statuses reset to unknown");
}
