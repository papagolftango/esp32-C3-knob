#pragma once

// Common data structures used across features

// Energy monitoring data
struct EnergyData {
    float balance = 0.0f;      // Current energy balance (kW)
    float solar = 0.0f;        // Solar generation (kW)
    float used = 0.0f;         // Energy consumption (kW)
    float vrms = 0.0f;         // RMS voltage
    int tariff = 1;            // Current tariff (1-4)
    bool valid = false;        // Data validity flag
};

// Daily peak tracking data
struct PeakData {
    float daily_import_peak = 0.0f;    // Highest import (negative balance)
    float daily_export_peak = 0.0f;    // Highest export (positive balance)
    unsigned long last_peak_update = 0; // When peaks were last updated
    bool import_peak_reached_today = false;
    bool export_peak_reached_today = false;
};

// Connection status
struct ConnectionStatus {
    bool wifi_connected = false;
    bool mqtt_connected = false;
};

// Screen management
enum Screen {
    SCREEN_ENERGY = 0,
    SCREEN_WEATHER,
    SCREEN_HOUSE_INFO,
    SCREEN_SETTINGS,
    SCREEN_COUNT  // Total number of screens
};

// Settings menu state
struct SettingsState {
    int selected_item = 0;
    bool menu_active = false;
    static const int MENU_ITEMS = 3;
};
