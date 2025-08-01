# Energy Monitor with Peak Tracking - Enhanced Design

## Display Layout

The Energy screen combines a **large balance arc** with **mini solar/usage arcs** and **daily peak indicators**:

```
⚡ ENERGY 🟢     ← Title with tariff LED indicator

 ☀️              ╭─────────╮              🏠
╭──╮           ╱             ╲           ╭──╮
│2.5│         ╱    EXPORT     ╲          │1k│
│k │         │     1500W      │         │W │
╰──╯          ╲               ╱          ╰──╯
               ╲_____●_____╱
                     ↑
               Peak export dot (green)

     📡 EmonTX3 | 240V      ← Connection & voltage
```

## Main Features

### 1. Large Balance Arc (Center)
- **Scale**: Export 0-4000W | Import 0-8000W  
- **Colors**: 
  - 🟢 Green: Good solar export (>1500W)
  - 🟠 Orange: Low solar export (≤1500W)
  - 🔴 Red: Grid import
  - ⚪ Grey: Balanced
- **Center Text**: EXPORT/IMPORT/BALANCED with power value

### 2. Daily Peak Indicators
- **Green Dot**: Daily peak export position on arc
- **Red Dot**: Daily peak import position on arc
- **Reset Time**: Daily at 12:00 noon (configurable in settings)
- **Persistence**: Peaks tracked throughout the day

### 3. Mini Arcs (Left & Right)
- **Left (Yellow)**: ☀️ Solar generation (0-5000W scale)
- **Right (Blue)**: 🏠 House usage (0-8000W scale)
- **Half-circle design** for compact display

### 4. Tariff LED Indicator
- **🟢 Green**: Low/Off/Night tariff
- **🔴 Red**: High/Peak tariff

## Peak Tracking Logic

### Daily Reset
- **Time**: 12:00 noon each day
- **Action**: Reset both `daily_peak_export` and `daily_peak_import` to 0
- **Configurable**: Can be adjusted in settings

### Peak Updates
- **Export Peak**: Most negative balance value of the day
- **Import Peak**: Most positive balance value of the day
- **Real-time**: Updates immediately when new peak reached

### Dot Positioning
- **Angle Calculation**: Peak value / max_scale * 360°
- **Visual Feedback**: White border around colored dots
- **Persistent**: Dots remain visible throughout the day

## Example Scenarios

### Morning - Building Solar
```
⚡ ENERGY 🟢

 ☀️              ╭─────────╮              🏠
╭──╮           ╱    EXPORT   ╲           ╭──╮
│1.2│         │     800W     │          │1.5│
│k │         ╲               ╱          │k │
╰──╯          ╲_____●_____╱             ╰──╯
               Peak: 2.5kW export
```

### Evening - Grid Import
```
⚡ ENERGY 🔴

 ☀️              ╭─────────╮              🏠
╭──╮           ╱    IMPORT   ╲           ╭──╮
│0  │         │    1200W     │          │3k│
│W │         ╲       ●       ╱          │W │
╰──╯          ╲_____________╱            ╰──╯
               Peak: 3.2kW import
```

## MQTT Integration

### Topics Monitored
- `emon/emontx3/balance` → Updates balance + peak tracking
- `emon/emontx3/solar` → Updates solar mini arc
- `emon/emontx3/used` → Updates usage mini arc
- `emon/emontx3/vrms` → Small voltage display
- `emon/emontx3/tariff` → LED indicator

### Peak Reset Command
```bash
# Manual peak reset (via settings or command)
mosquitto_pub -h your-broker -t "home/knob/command" -m "reset_peaks"
```

## Benefits

1. **Historical Context**: See daily peaks vs current values
2. **Visual Trends**: Understand daily energy patterns
3. **Comprehensive View**: Balance + solar + usage in one screen
4. **Real-time + History**: Current data with peak references

Your energy display now provides both real-time monitoring and daily peak tracking! 🎯⚡
