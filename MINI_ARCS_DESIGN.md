# EmonTX3 Energy Monitor Integration - Mini Arcs Design

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

## Energy Screen Display - Two Mini Arcs Design

The Energy screen features **two mini arcs** plus central balance display:

### Layout Design
```
⚡ ENERGY 🟢     ← Title with tariff LED indicator

   ☀️       EXPORT      🏠
 ╭───╮      1500W     ╭───╮
╱ 2.5k ╲   (GREEN)    ╱ 1k ╲   ← Mini arcs for solar & usage
╲ W   ╱               ╲ W  ╱
 ╰───╯                 ╰───╯

     📡 EmonTX3 | 240V      ← Connection & voltage
```

### Two Mini Arcs
- **Left Arc (Yellow)**: ☀️ Solar Generation
  - Scale: 0-5000W maximum
  - Half-circle display
  - Shows solar power + value
- **Right Arc (Blue)**: 🏠 House Usage  
  - Scale: 0-8000W maximum
  - Half-circle display  
  - Shows usage power + value

### Central Balance Display
- **EXPORT** (negative): Color based on solar
  - 🟢 Green text if solar >1500W
  - 🟠 Orange text if solar ≤1500W
- **IMPORT** (positive): 🔴 Red text
- **BALANCED**: ⚪ Grey text

### Tariff LED Indicator
- **🟢 Green Circle**: Low/Off/Night tariff
- **🔴 Red Circle**: High/Peak tariff
- **No Circle**: No tariff data

## Visual Elements
1. **Yellow Arc**: Solar generation (always yellow)
2. **Blue Arc**: House usage (always blue)  
3. **Balance Text**: Smart color coding
   - Green: Good solar export
   - Orange: Low solar export
   - Red: Grid import
   - Grey: Balanced

## Example Scenarios

### High Solar Export
```
⚡ ENERGY 🟢

   ☀️       EXPORT      🏠
 ╭───╮      2500W     ╭───╮
╱ 3.5k ╲   (GREEN)    ╱ 1k ╲
╲ W   ╱               ╲ W  ╱
 ╰───╯                 ╰───╯

📡 EmonTX3 | 240V
```

### Grid Import  
```
⚡ ENERGY 🔴

   ☀️       IMPORT      🏠
 ╭───╮      1500W     ╭───╮
╱ 500 ╲    (RED)      ╱ 2k ╲
╲ W   ╱               ╲ W  ╱
 ╰───╯                 ╰───╯

📡 EmonTX3 | 240V
```

### Balanced System
```
⚡ ENERGY 🟢

   ☀️      BALANCED     🏠
 ╭───╮       0W       ╭───╮
╱ 2k  ╲   (GREY)      ╱ 2k ╲
╲ W   ╱               ╲ W  ╱
 ╰───╯                 ╰───╯

📡 EmonTX3 | 240V
```

## Testing Commands
Use your MQTT client to test the integration:

```bash
# Test energy balance (importing 1.5kW)
mosquitto_pub -h your-broker -t "emon/emontx3/balance" -m "1500"

# Test solar generation
mosquitto_pub -h your-broker -t "emon/emontx3/solar" -m "2500" 

# Test consumption
mosquitto_pub -h your-broker -t "emon/emontx3/used" -m "1800"

# Test voltage
mosquitto_pub -h your-broker -t "emon/emontx3/vrms" -m "240"

# Test low tariff
mosquitto_pub -h your-broker -t "emon/emontx3/tariff" -m "low"

# Test high tariff  
mosquitto_pub -h your-broker -t "emon/emontx3/tariff" -m "high"
```

The mini arcs provide instant visual feedback on solar generation and house consumption! 🎯⚡
