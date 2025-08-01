# Mock Mode Demo Guide 🎯

## Quick Start

Your ESP32-S3 knob now has **built-in demo mode** to preview the energy monitoring interface without needing EmonTX3 hardware!

## Enabling Mock Mode

### Option 1: Code Configuration (Current)
```cpp
#define ENABLE_MOCK_DATA true  // Set to false when using real MQTT data
```

### Option 2: Settings Screen
- Navigate to **Settings** screen (rotate knob)
- View current mock mode status: `Mock Data: ON 🟢`
- Status displayed: `🎯 DEMO MODE ACTIVE`

## What You'll See

### Realistic Energy Simulation
The mock mode simulates a full 24-hour energy cycle in **2 minutes**:

#### 🌅 Morning (6:00-10:00)
- ☀️ Solar: Building from 0W to 2000W
- 🏠 Usage: High (morning routines) ~2300W
- ⚡ Balance: Import from grid (red)

#### ☀️ Midday (10:00-16:00) 
- ☀️ Solar: Peak generation ~4500W
- 🏠 Usage: Moderate ~800W base load
- ⚡ Balance: **Export to grid** (green)
- 🟢 **Peak export tracking**: Green dot on arc

#### 🌅 Evening (16:00-22:00)
- ☀️ Solar: Declining production
- 🏠 Usage: High (evening activities) ~2800W
- ⚡ Balance: Import from grid (red)
- 🔴 **Peak import tracking**: Red dot on arc
- 🔴 **Peak tariff**: Red LED indicator

#### 🌙 Night (22:00-6:00)
- ☀️ Solar: 0W (no generation)
- 🏠 Usage: Low (base load) ~400W
- ⚡ Balance: Grid import (minimal)

### Peak Tracking Demo
- **Green dots**: Daily peak export positions
- **Red dots**: Daily peak import positions
- **Reset**: Every 24 hours (simulated at noon)
- **Real-time updates**: Peaks update as values change

### UI Features Demonstrated
- **Large balance arc**: Color-coded energy flow
- **Mini solar arc**: Yellow, left side
- **Mini usage arc**: Blue, right side
- **Tariff LED**: 🟢 Standard / 🔴 Peak rates
- **Connection status**: Simulated MQTT connected
- **Voltage display**: ~240V ±5V variation

## Development Benefits

### 1. Visual Testing
- See all energy scenarios (export/import/balanced)
- Verify peak tracking dot positioning
- Test UI responsiveness and animations
- Validate color coding and thresholds

### 2. UI Development
- Rapid iteration without hardware dependencies
- Test edge cases (high solar, peak demand)
- Verify arc calculations and scaling
- Check text positioning and readability

### 3. Demo Capabilities
- Show clients the interface before hardware arrives
- Demonstrate daily energy patterns
- Explain peak tracking functionality
- Preview cumulative daily displays (future feature)

## Technical Details

### Data Generation
```cpp
// Solar: Sine wave peaked at noon
solar_factor = sin((mock_time_offset - 6) * PI / 12);
energy_solar = solar_factor * 4500W + noise;

// Usage: Base + morning/evening peaks
energy_used = 800W + morning_spike + evening_spike;

// Balance: Import/export calculation
energy_balance = energy_used - energy_solar;
```

### Update Frequency
- **UI Updates**: Every 2 seconds
- **Peak Tracking**: Real-time with each update
- **Debug Output**: Every 10 seconds
- **Daily Reset**: Every 24 simulated hours

## Switching to Real Data

When your EmonTX3 arrives:

1. **Change Configuration**:
   ```cpp
   #define ENABLE_MOCK_DATA false
   ```

2. **Configure MQTT**:
   - Use WiFiManager to set broker details
   - Enter EmonTX3 topic credentials
   - Mock mode will automatically disable

3. **Verify Topics**:
   - `emon/emontx3/balance`
   - `emon/emontx3/solar`
   - `emon/emontx3/used`
   - `emon/emontx3/vrms`
   - `emon/emontx3/tariff`

## Future: Cumulative Displays

The current mini arcs show **instantaneous power**. Your planned enhancement:

- **Daily kWh totals** instead of current Watts
- **Cumulative progress bars** or **daily targets**
- **Different scales**: kWh instead of W
- **Reset timing**: Daily at midnight/noon

The mock mode framework will easily adapt to simulate cumulative data!

---

**Ready to see your energy monitoring interface in action!** 🚀⚡
