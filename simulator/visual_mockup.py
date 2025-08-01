#!/usr/bin/env python3
"""
LVGL Energy Screen Visual Mockup Generator
Creates ASCII art representation of the 360x360 energy monitoring interface
"""

import math
import os

def create_energy_screen_mockup(balance=-1200, solar=2500, usage=1000, voltage=240, tariff="standard"):
    """Generate ASCII art mockup of energy screen"""
    
    # Determine balance status
    if balance < -1500:
        balance_status = "EXPORT"
        balance_color = "🟢"  # Good export
        balance_symbol = "⬇️"
    elif balance < 0:
        balance_status = "EXPORT" 
        balance_color = "🟡"  # Low export
        balance_symbol = "⬇️"
    elif balance > 0:
        balance_status = "IMPORT"
        balance_color = "🔴"  # Import
        balance_symbol = "⬆️"
    else:
        balance_status = "BALANCED"
        balance_color = "⚪"  # Balanced
        balance_symbol = "⚖️"
    
    # Format values
    def format_power(watts):
        if abs(watts) >= 1000:
            return f"{watts/1000:.1f}k"
        else:
            return f"{watts:.0f}"
    
    balance_value = abs(balance)
    solar_display = format_power(solar)
    usage_display = format_power(usage)
    balance_display = format_power(balance_value)
    
    # Tariff indicator
    tariff_led = "🔴" if tariff == "peak" else "🟢"
    
    # Create the mockup
    mockup = f"""
┌─────────────────────────────────────────────────────────────┐
│                    360 x 360 Display Preview                │
├─────────────────────────────────────────────────────────────┤
│  ⚡ ENERGY {tariff_led}                    1/4 - Rotate │
│                                                             │
│   ☀️                ╭─────────────╮               🏠        │
│  ╭────╮            ╱               ╲            ╭────╮      │
│  │{solar_display:^4}│          ╱     {balance_status:^9}     ╲          │{usage_display:^4}│      │
│  │ W  │         │      {balance_display:^7}W     │         │ W  │      │
│  ╰────╯          ╲       {balance_symbol}        ╱          ╰────╯      │
│   30%             ╲_______●_______╱              85%        │
│ Solar Arc           Peak dots              Usage Arc        │
│                     🟢 Export                               │
│                     🔴 Import                               │
│                                                             │
│                📡 EmonTX3 | {voltage:.0f}V                       │
│                                                             │
│                   Touch to interact                        │
│                                                             │
│ Arc Details:                                                │
│ • Balance: {balance_color} {balance:+.0f}W ({balance_status})                   │
│ • Solar:   🟡 {solar:.0f}W generation                          │
│ • Usage:   🔵 {usage:.0f}W consumption                         │
│ • Tariff:  {tariff_led} {tariff.title()} rate                              │
└─────────────────────────────────────────────────────────────┘
"""
    
    return mockup

def create_all_screen_mockups():
    """Create mockups for all 4 screens"""
    
    screens = {
        "Energy": create_energy_screen_mockup(),
        "Weather": create_weather_mockup(),
        "House Info": create_house_info_mockup(), 
        "Settings": create_settings_mockup()
    }
    
    return screens

def create_weather_mockup():
    return """
┌─────────────────────────────────────────────────────────────┐
│                    360 x 360 Display Preview                │
├─────────────────────────────────────────────────────────────┤
│  🌤️ WEATHER                       2/4 - Rotate │
│                                                             │
│                       ☀️                                    │
│                     22°C                                   │
│                   Sunny                                     │
│                                                             │
│               ╭─────────────╮                              │
│              ╱               ╲                             │
│             │    UV Index     │                            │
│            │       6/10       │                           │
│             ╲                ╱                             │
│              ╲_______________╱                              │
│                                                             │
│  Wind: 12 km/h NW    Humidity: 45%                        │
│  Feels like: 24°C    Pressure: 1013 hPa                   │
│                                                             │
│                   Touch to interact                        │
└─────────────────────────────────────────────────────────────┘
"""

def create_house_info_mockup():
    return """
┌─────────────────────────────────────────────────────────────┐
│                    360 x 360 Display Preview                │
├─────────────────────────────────────────────────────────────┤
│  🏠 HOUSE INFO                    3/4 - Rotate │
│                                                             │
│                                                             │
│              Daily Energy Summary                           │
│                                                             │
│               ╭─────────────╮                              │
│              ╱               ╲                             │
│             │   Solar Gen     │                            │
│            │    25.4 kWh      │                           │
│             ╲                ╱                             │
│              ╲_______________╱                              │
│                                                             │
│  Grid Export: 8.2 kWh    Grid Import: 2.1 kWh            │
│  Self Use: 17.2 kWh      Battery: 95%                     │
│  Peak Export: 4.2kW      Peak Import: 1.8kW               │
│                                                             │
│                   Touch to interact                        │
└─────────────────────────────────────────────────────────────┘
"""

def create_settings_mockup():
    return """
┌─────────────────────────────────────────────────────────────┐
│                    360 x 360 Display Preview                │
├─────────────────────────────────────────────────────────────┤
│  ⚙️ SETTINGS                      4/4 - Rotate │
│                                                             │
│                                                             │
│                    Configuration                            │
│                                                             │
│               • Brightness: 80%                            │
│               • Mock Data: ON 🟢                           │
│               • Reset WiFi                                 │
│               • Factory Reset                              │
│               • Display Info                               │
│                                                             │
│                                                             │
│                🎯 DEMO MODE ACTIVE                         │
│               Simulating EmonTX3 data                      │
│                                                             │
│                   Touch to interact                        │
└─────────────────────────────────────────────────────────────┘
"""

def simulate_daily_cycle():
    """Simulate 24 hours of energy data"""
    
    print("🎬 Daily Energy Cycle Simulation (24 hours in 30 seconds)")
    print("=" * 65)
    
    import time
    
    for hour in range(0, 24, 2):  # Every 2 hours
        # Calculate solar (sine wave, peak at noon)
        solar_factor = max(0, math.sin((hour - 6) * math.pi / 12))
        solar = solar_factor * 4500
        
        # Calculate usage (higher morning/evening)
        usage_base = 800
        if 6 <= hour <= 10:  # Morning
            usage = usage_base + 1500
        elif 17 <= hour <= 21:  # Evening  
            usage = usage_base + 2000
        elif hour <= 5 or hour >= 23:  # Night
            usage = usage_base - 400
        else:
            usage = usage_base
            
        # Calculate balance
        balance = usage - solar
        
        # Determine tariff
        tariff = "peak" if 16 <= hour <= 20 else "standard"
        
        print(f"\n⏰ {hour:02d}:00")
        print(create_energy_screen_mockup(balance, solar, usage, 240, tariff))
        
        time.sleep(1.2)  # 1.2 seconds per 2-hour period = ~30 seconds total

if __name__ == "__main__":
    print("🎯 ESP32-S3 Energy Knob - Visual Mockup Generator")
    print("=" * 60)
    
    # Show current state
    print("\n1. Current Energy Screen:")
    print(create_energy_screen_mockup())
    
    # Show all screens
    print("\n2. All Screen Mockups:")
    screens = create_all_screen_mockups()
    for name, mockup in screens.items():
        print(f"\n📱 {name} Screen:")
        print(mockup)
    
    # Ask for daily simulation
    response = input("\n🎬 Run daily energy cycle simulation? (y/n): ")
    if response.lower() == 'y':
        simulate_daily_cycle()
    
    print("\n✅ Mockup generation complete!")
    print("💡 Tip: Use this to visualize your energy monitoring interface!")
