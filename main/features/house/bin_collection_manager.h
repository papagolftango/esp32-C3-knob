#pragma once

#include <string>
#include <ctime>

enum class BinType {
    BLACK = 0,
    ORANGE = 1,
    GREEN = 2,
    NONE = 3
};

enum class BinCollectionStatus {
    PREPARE_TONIGHT,   // Collection tomorrow - prepare bins tonight
    DUE_TODAY,         // Collection is today
    NEXT_COLLECTION,   // Shows when next collection is due
    UNKNOWN            // Status not determined
};

struct BinCollectionData {
    BinType bin_type;
    BinCollectionStatus status;
    int days_until_next;       // Days until next collection
    std::string next_collection_day; // Next collection day (e.g., "Tuesday")
};

class BinCollectionManager {
public:
    static void begin();
    
    // Data access methods
    static BinCollectionData getBlackBinData() { return black_bin; }
    static BinCollectionData getOrangeBinData() { return orange_bin; }
    static BinCollectionData getGreenBinData() { return green_bin; }
    
    // Status check methods
    static bool hasDataChanged();
    static bool isDataValid();
    
    // Get current active collection info for display
    static BinType getTodaysCollection();
    static std::string getDisplayStatus();
    
    // External schedule update (called at midnight with new schedule data)
    static void updateSchedule(BinType tomorrow_bin, BinType today_bin, int days_to_next);
    
    // Utility methods
    static std::string binTypeToString(BinType type);
    static std::string statusToString(BinCollectionStatus status);

private:
    // Bin data
    static BinCollectionData black_bin;
    static BinCollectionData orange_bin;
    static BinCollectionData green_bin;
    
    // State tracking
    static bool data_changed;
    static unsigned long last_update;
    
    // Internal methods
    static void resetAllStatus();
};
