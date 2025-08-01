# EmonTX3 Energy Monitor Integration

## MQTT Topics Configured

Your ESP32-S3 knob now subscribes to your EmonTX3 energy monitoring topics:

### Energy Data Topics
- **`emon/emontx3/balance`** → Grid import/export balance (W)
- **`emon/emontx3/solar`** → Solar generation (W)  
- **`emon/emontx3/vrms`** → Voltage RMS (V)
- **`emon/emontx3/used`** → Energy consumption (W)
- **`emon/emontx3/tariff`** → Current tariff status

### Control Topic
- **`home/knob/command`** → Device control commands

## Energy Screen Display

The Energy screen focuses on the **BALANCE** as the main indicator:

### Arc Scaling & Colors
- **EXPORT** (negative balance):
  - Scale: 0 to 4000W maximum
  - Color: 🟢 **Green** if solar >1500W, � **Orange** if solar ≤1500W
- **IMPORT** (positive balance):
  - Scale: 0 to 8000W maximum  
  - Color: 🔴 **Red** (importing from grid)
- **BALANCED**: 
  - Arc: 0%
  - Color: ⚪ **Grey** (neutral)

### Display Layout
```
⚡ ENERGY 🟢     ← Title with tariff LED indicator

     ╭─────╮
    ╱       ╲     ← Colored arc shows balance magnitude
   ╱  EXPORT  ╲   ← Center shows IMPORT/EXPORT/BALANCED
  │   1500W    │  ← with power value
   ╲         ╱
    ╲_______╱

☀️2500W            🏠1000W  ← Solar (left) | Usage (right)

     📡 EmonTX3 | 240V      ← Connection & small voltage
```

### Tariff LED Indicator
- **🟢 Green Circle**: Low/Off/Night tariff
- **🔴 Red Circle**: High/Peak tariff
- **No Circle**: No tariff data

### Data Positioning
- **Center Arc**: Main balance (IMPORT/EXPORT/BALANCED)
- **Bottom Left**: ☀️ Solar generation
- **Bottom Right**: 🏠 House usage
- **Bottom Center**: Connection status + small voltage number

## Color Logic
1. **Green Arc**: Exporting with good solar (>1500W)
2. **Orange Arc**: Exporting with low solar (≤1500W)
3. **Red Arc**: Importing from grid
4. **Grey Arc**: Balanced (net zero)

## Example Scenarios

### High Solar Export
```
⚡ ENERGY 🟢

   EXPORT
   2500W
   
☀️3500W     🏠1000W
📡 EmonTX3 | 240V
```

### Grid Import  
```
⚡ ENERGY 🔴

   IMPORT
   1500W
   
☀️500W      🏠2000W
📡 EmonTX3 | 240V
```

### Balanced
```
⚡ ENERGY 🟢

  BALANCED
     0W
     
☀️2000W     🏠2000W
📡 EmonTX3 | 240V
```

Your ESP32-S3 knob now provides a focused energy balance display! 🎯⚡
