Feature: System Integration and Overall Behaviour
  As a user
  I want all system components to work together seamlessly
  So that I have a reliable and integrated smart home interface

  Background:
    Given the ESP32-S3 Knob device is fully initialized
    And all components are running correctly

  Scenario: Complete system startup sequence
    Given the device is powered on
    When the initialisation sequence completes
    Then the hardware should be initialized (display, rotary encoder, WiFi)
    And LVGL graphics system should be ready
    And WiFi should connect to configured network
    And MQTT should connect to configured broker
    And all screen managers should be initialized
    And the hello world screen should be displayed
    And the system should be ready for user interaction

  Scenario: Seamless screen navigation throughout system
    Given the system is fully operational
    When I navigate through all screens in sequence
    Then the transition should be: Hello → Clock → Energy → Weather → House → Settings → (wrap to Hello)
    And each transition should be smooth with LVGL animations
    And each screen should display its content correctly
    And rotary encoder input should be responsive on all screens

  Scenario: Multi-component data flow integration
    Given EmonTX3 is sending energy data via MQTT
    And home automation system is sending MOTD updates
    And bin collection schedule is available
    When I navigate to the energy screen
    Then real-time energy data should be displayed with daily peaks
    When I navigate to the house screen
    Then bin collection status should be visually indicated
    And MOTD ticker should scroll with current message
    And both systems should operate independently without interference

  Scenario: MQTT message handling across all components
    Given MQTT is connected and all handlers are registered
    When messages arrive on different topic namespaces:
      | Namespace | Component | Expected Behaviour |
      | emon/emontx3/* | Energy Manager | Updates energy display |
      | home/knob/* | Device Control | Changes device settings |
      | home/motd | MOTD Manager | Updates ticker message |
    Then each message should be routed to correct component
    And components should not interfere with each other
    And all updates should be reflected in appropriate screens

  Scenario: Timing and animation coordination
    Given the house screen is active with bins in PREPARE_TONIGHT status
    And MOTD ticker has a long message requiring scrolling
    When both animations are running simultaneously
    Then bin icons should flash every 500ms
    And MOTD text should scroll every 80ms
    And these animations should run independently
    And system performance should remain responsive
    And no timing conflicts should occur

  Scenario: Connection status monitoring across screens
    Given I am viewing any screen in the system
    When WiFi connection is lost
    Then the status arc should turn red on all screens
    And this should be consistent across screen transitions
    When WiFi reconnects but MQTT is still disconnected
    Then the status arc should turn orange on all screens
    When both WiFi and MQTT are connected
    Then the status arc should turn green on all screens

  Scenario: System resource management
    Given all features are active simultaneously (energy monitoring, bin collection, MOTD, settings)
    When the system is operating at full capacity
    Then memory usage should remain within acceptable limits
    And display refresh rate should be maintained
    And MQTT message processing should not be delayed
    And user input should remain responsive
    And no memory leaks should occur over extended operation

  Scenario: Error handling and system resilience
    Given the system is running normally
    When one component encounters an error (e.g., invalid MQTT data)
    Then other components should continue operating normally
    And the error should be logged appropriately
    And the system should not crash or become unresponsive
    And recovery should be automatic when possible

  Scenario: Daily reset coordination
    Given it's approaching midnight with accumulated energy data
    When the daily reset occurs at 00:00:00
    Then energy peak and accumulation should reset
    And bin collection statuses should be evaluated for new day
    And all other systems should continue operating normally
    And the reset should be logged across all relevant components

  Scenario: Settings persistence and system-wide application
    Given device settings are changed (brightness, haptic)
    When the changes are made via settings screen
    Then the changes should apply immediately system-wide
    And MQTT status should be published
    When the changes are made via MQTT commands
    Then the settings screen should reflect the changes
    And the actual device behaviour should change accordingly

  Scenario: System performance under load
    Given all features are active and processing data
    When high-frequency MQTT messages are received
    And user is actively navigating between screens
    And animations are running (flashing bins, scrolling text)
    Then the system should maintain stable operation
    And screen transitions should remain smooth
    And no data should be lost or corrupted
    And user interface should remain responsive

  Scenario: Power cycle and state recovery
    Given the system has been running with accumulated data
    When the device is power cycled (reboot)
    Then WiFi credentials should be restored
    And MQTT configuration should be restored
    And device settings (brightness, haptic) should be restored
    And the system should return to full operational state
    And all features should resume normal operation
    And daily accumulations should reset if new day has started
