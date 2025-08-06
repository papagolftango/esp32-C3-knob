#pragma once

#include <string>

/**
 * Message Of The Day Manager
 * 
 * Handles MQTT MOTD messages for display as scrolling ticker
 */
class MOTDManager {
public:
    // Initialize MOTD manager
    static void init();
    
    // Update MOTD message from MQTT
    static void updateMessage(const char* message);
    
    // Get current MOTD message
    static std::string getCurrentMessage();
    
    // Check if MOTD has changed (for UI updates)
    static bool hasMessageChanged();
    
    // Clear change flag after UI update
    static void clearChangeFlag();
    
    // Get default message when no MOTD available
    static std::string getDefaultMessage();

private:
    static std::string current_message;
    static bool message_changed;
    static bool initialized;
};
