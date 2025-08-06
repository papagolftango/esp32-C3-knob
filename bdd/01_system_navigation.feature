Feature: ESP32-S3 Knob System Navigation
  As a user
  I want to navigate between different screens using the rotary encoder
  So that I can access all device functions

  Background:
    Given the ESP32-S3 Knob device is powered on
    And WiFi connection is established
    And MQTT client is connected

  Scenario: System startup and initial screen display
    Given the device has just booted
    When the initialisation completes
    Then the hello world screen should be displayed
    And the WiFi/MQTT connection status arc should be visible
    And the connection status should show green (WiFi + MQTT connected)

  Scenario: Navigating between screens using rotary encoder
    Given I am on any screen
    When I rotate the encoder clockwise
    Then the system should advance to the next screen in sequence
    And the transition should be smooth with LVGL animation

  Scenario: Screen sequence navigation
    Given I am on the hello world screen
    When I rotate through all screens
    Then the sequence should be:
      | Screen Order | Screen Name |
      | 1           | Hello World |
      | 2           | Clock       |
      | 3           | Energy      |
      | 4           | Weather     |
      | 5           | House       |
      | 6           | Settings    |
    And after the settings screen, it should wrap back to hello world

  Scenario: Button press handling on each screen
    Given I am on any screen
    When I press the rotary encoder button
    Then the screen-specific button handler should be called
    And appropriate feedback should be provided

  Scenario: Connection status indicator behaviour
    Given I am viewing any screen
    When the WiFi connection is active and MQTT is connected
    Then the status arc should be green
    When the WiFi is connected but MQTT is disconnected
    Then the status arc should be orange
    When the WiFi is disconnected
    Then the status arc should be red
    And the connection details should update accordingly
