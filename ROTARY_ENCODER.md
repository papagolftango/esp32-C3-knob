# Rotary Encoder Integration

## Pin Configuration

The ESP32-C3 Knob now supports rotary encoder navigation with the following pin assignments:

| Pin | Function | GPIO |
|-----|----------|------|
| ENCODER_PIN_A | Rotary A signal | GPIO8 |
| ENCODER_PIN_B | Rotary B signal | GPIO9 |
| ENCODER_BTN | Encoder button | GPIO4 |

## Navigation Controls

### **Rotary Encoder:**
- **Clockwise rotation** → Next screen (Energy → Weather → House Info → Settings → Energy...)
- **Counter-clockwise rotation** → Previous screen (Energy ← Weather ← House Info ← Settings ← Energy...)
- **Button press** → Next screen (alternative to rotation)
- **Long press (1 sec)** → Reserved for future settings access

### **Touch Screen (Backup):**
- **Touch anywhere** → Next screen (500ms debounce)

## Screen Order

1. **⚡ Energy** (Default)
2. **🌤️ Weather**
3. **🏠 House Info**
4. **⚙️ Settings**

## Technical Implementation

### **Interrupt-based Encoder Reading:**
- Uses hardware interrupts on both A and B pins for reliable detection
- Quadrature encoding for direction determination
- Built-in debouncing (5ms hardware, 200ms navigation)
- Position tracking with hysteresis (requires 2 steps to trigger navigation)

### **Button Handling:**
- Active-low button with internal pullup
- 200ms debounce for press detection
- Long press detection (1000ms threshold)
- Suitable for future multi-function use

## Hardware Requirements

### **Rotary Encoder Specifications:**
- Standard quadrature rotary encoder
- 3.3V compatible
- Optional: Encoder with integrated push button
- Common types: KY-040, EC11, or similar

### **Wiring:**
```
ESP32-C3     Rotary Encoder
--------     --------------
GPIO8    →   Pin A (CLK)
GPIO9    →   Pin B (DT)
GPIO4    →   Button (SW)
3.3V     →   VCC/+
GND      →   GND/-
```

## Pin Selection Rationale

The chosen pins (GPIO 4, 8, 9) were selected because:
- ✅ **Available on ESP32-C3** - Not used by display or other peripherals
- ✅ **Interrupt capable** - All can trigger hardware interrupts
- ✅ **No conflicts** - Avoid display pins (0-3, 5-6, 10) and reserved pins
- ✅ **Good electrical characteristics** - Suitable for encoder signals

## Future Enhancements

### **Planned Features:**
- **Variable sensitivity** - Adjust rotation speed requirements
- **Context-sensitive button** - Different actions per screen
- **Value adjustment** - Use rotation for brightness, volume, etc.
- **Menu navigation** - Navigate within settings menus

### **Advanced Options:**
- **Acceleration** - Faster rotation = bigger jumps
- **Haptic feedback** - Vibration motor support
- **Multi-press detection** - Double-click, triple-click patterns
- **Rotation value display** - Show parameter adjustments

## Usage Examples

### **Basic Navigation:**
```cpp
// Rotate clockwise
Energy → Weather → House Info → Settings → Energy...

// Rotate counter-clockwise  
Energy ← Weather ← House Info ← Settings ← Energy...
```

### **Button Actions:**
```cpp
// Short press: Next screen
// Long press: Enter/exit settings mode (future)
// Double press: Quick settings (future)
```

## Debugging

### **Serial Monitor Output:**
```
Rotary encoder initialized on pins A:8 B:9 BTN:4
Rotary: Next screen (CW)
Switched to screen: Weather
Rotary: Previous screen (CCW)
Switched to screen: Energy
Encoder button pressed
```

### **Troubleshooting:**
- **No rotation detected:** Check wiring and pullup resistors
- **Wrong direction:** Swap A and B pins
- **Bouncy behavior:** Encoder may need hardware debouncing capacitors
- **Missed steps:** Reduce rotation speed or adjust sensitivity

The rotary encoder provides intuitive, tactile navigation perfect for the knob form factor! 🎛️
