Feature: Settings Screen
  As a user
  I want to configure device settings like brightness and haptic feedback
  So that I can customize the device to my preferences

  Background:
    Given the ESP32-S3 Knob device is initialized
    And I have navigated to the settings screen

  Scenario: Settings screen display
    Given the settings screen is loaded
    When the screen is displayed
    Then I should see "Settings" as the screen title
    And I should see the current brightness level displayed
    And I should see the current haptic feedback status
    And the display should be consistent with other screens

  Scenario: Brightness level display
    Given the settings screen is active
    When brightness is set to a specific level
    Then I should see "Brightness: X%" where X is the current level
    And the percentage should accurately reflect the actual brightness
    And the display should update when brightness changes

  Scenario: Haptic feedback status display
    Given the settings screen is active
    When haptic feedback is enabled
    Then I should see "Haptic: ON" 
    When haptic feedback is disabled
    Then I should see "Haptic: OFF"
    And the status should accurately reflect the current setting

  Scenario: Settings screen navigation
    Given I am on the settings screen
    When I rotate the encoder clockwise
    Then the system should wrap around to the hello world screen
    When I rotate the encoder counter-clockwise
    Then the system should transition to the house screen

  Scenario: Button press cycling brightness
    Given I am on the settings screen
    When I press the rotary encoder button
    Then the brightness should cycle to the next level
    And the brightness levels should follow the sequence: 25%, 50%, 75%, 100%
    And after 100%, it should wrap back to 25%
    And the display should update to show the new brightness level

  Scenario: Brightness cycling sequence
    Given the current brightness is 25%
    When I press the button
    Then brightness should change to 50%
    Given the current brightness is 50%
    When I press the button
    Then brightness should change to 75%
    Given the current brightness is 75%
    When I press the button
    Then brightness should change to 100%
    Given the current brightness is 100%
    When I press the button
    Then brightness should wrap back to 25%

  Scenario: Haptic feedback toggling
    Given the settings screen supports haptic control
    When the haptic toggle functionality is activated
    Then the haptic feedback should toggle between ON and OFF
    And the status display should update accordingly
    And the actual haptic feedback should be enabled/disabled

  Scenario: MQTT status publishing for brightness changes
    Given MQTT is connected
    When brightness is changed via the settings screen
    Then a status message should be published to "home/knob/brightness/status"
    And the message should contain the new brightness percentage
    And the update should be immediate

  Scenario: MQTT status publishing for haptic changes
    Given MQTT is connected
    When haptic feedback is toggled via the settings screen
    Then a status message should be published to "home/knob/haptic/status"
    And the message should contain "on" or "off"
    And the update should be immediate

  Scenario: MQTT command reception for brightness
    Given the settings screen is active
    When an MQTT message is received on "home/knob/brightness"
    Then the brightness should change to the specified level
    And the settings screen display should update
    And the new value should be within the valid range (0-100)

  Scenario: MQTT command reception for haptic
    Given the settings screen is active
    When an MQTT message is received on "home/knob/haptic"
    Then the haptic feedback should be enabled/disabled as specified
    And the settings screen display should update
    And valid values should be "on", "off", "true", "false", or "toggle"

  Scenario: Settings persistence
    Given settings have been changed
    When the device is restarted
    Then the brightness setting should be restored
    And the haptic setting should be restored
    And the settings screen should display the correct values

  Scenario: Invalid settings handling
    Given invalid settings are received via MQTT
    When brightness value is outside 0-100 range
    Then the value should be clamped to valid range
    When haptic value is not recognised
    Then the current setting should be maintained
    And error conditions should be logged appropriately
