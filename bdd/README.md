# ESP32-S3 Knob System - BDD Test Specifications

This directory contains Behavior-Driven Development (BDD) specifications for the ESP32-S3 Knob system. These files describe the expected behavior of each component and screen in a format that can be used for automated testing.

## 📋 BDD File Overview

### System Navigation and Core Functionality
- **01_system_navigation.feature** - Overall system navigation behavior and screen transitions
- **11_system_integration.feature** - Integration testing and overall system behavior

### Individual Screen Behaviors  
- **02_hello_world_screen.feature** - Welcome screen with connection status display
- **03_clock_screen.feature** - Time and date display functionality
- **04_energy_screen.feature** - Real-time energy monitoring and daily peak tracking
- **05_weather_screen.feature** - Weather information display (framework ready)
- **06_house_screen.feature** - Bin collection management and MOTD ticker
- **07_settings_screen.feature** - Device configuration (brightness, haptic feedback)

### Backend and Data Management
- **08_mqtt_integration.feature** - MQTT communication and message routing
- **09_bin_collection_management.feature** - Bin collection state management and visual indicators
- **10_energy_management.feature** - Energy data processing and peak tracking

## 🎯 Testing Strategy

### Gherkin Syntax
All BDD files use standard Gherkin syntax with:
- **Feature:** High-level description
- **Background:** Common setup for all scenarios
- **Scenario:** Specific test cases
- **Given/When/Then:** Test steps in natural language

### Coverage Areas
Each feature file covers:
- ✅ **Functional Requirements** - What the feature should do
- ✅ **Visual Behaviour** - How it should look and animate
- ✅ **Error Handling** - How it handles problems
- ✅ **Integration** - How it works with other components
- ✅ **Performance** - Timing and resource considerations

## 🚀 Usage

### For Development
- Use these specifications to guide feature implementation
- Ensure all described behaviors are implemented correctly
- Reference during code reviews to verify completeness

### For Testing
- Convert scenarios into automated tests using testing frameworks
- Use for manual testing validation
- Verify system behavior against documented specifications

### For Documentation
- These files serve as living documentation
- Describe system behavior in plain English
- Can be shared with non-technical stakeholders

## 🔍 Key Behavioural Specifications

### Screen Navigation
- Rotary encoder navigation through 6 screens
- Smooth LVGL transitions
- Wrap-around from last to first screen
- Button press handling per screen

### Energy Monitoring
- Real-time MQTT data processing
- Daily peak tracking with midnight reset
- kWh accumulation throughout the day
- Visual display updates

### House Management
- Bin collection visual indicators with state-based animations
- MOTD ticker with independent scrolling timing
- Multiple animation systems running simultaneously
- MQTT integration for real-time updates

### MQTT Integration
- Namespace-based message routing
- Auto-reconnection and error recovery
- Status publishing for device control
- Multiple concurrent subscriptions

## 📝 Maintenance

### Updating BDD Files
- Update scenarios when adding new features
- Modify existing scenarios when behaviour changes
- Add new files for major new components
- Keep scenarios focused and testable

### Validation
- Regularly verify that actual system behaviour matches BDD specifications
- Update specifications when requirements change
- Use BDD files to validate bug fixes and enhancements

---

**Note:** These BDD specifications describe the current implemented system as of August 2025, including MOTD ticker, bin collection management, energy monitoring with daily peaks, and complete MQTT integration.
