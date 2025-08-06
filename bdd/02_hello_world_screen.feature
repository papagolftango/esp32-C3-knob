Feature: Hello World Screen
  As a user
  I want to see the initial welcome screen with system status
  So that I know the device is working and connected

  Background:
    Given the ESP32-S3 Knob device is initialized
    And I am viewing the hello world screen

  Scenario: Initial hello world display
    Given the hello world screen is loaded
    Then I should see "Hello World!" text displayed
    And I should see "From ESP32-S3!" subtitle text
    And the text should be centered on the screen
    And the background should be dark theme appropriate

  Scenario: Connection status arc display
    Given the hello world screen is active
    When WiFi and MQTT are both connected
    Then the status arc should be displayed in green colour
    And the arc should indicate healthy connection status

  Scenario: Connection status arc - partial connectivity
    Given the hello world screen is active
    When WiFi is connected but MQTT is disconnected
    Then the status arc should be displayed in orange colour
    And this should indicate a warning state

  Scenario: Connection status arc - no connectivity
    Given the hello world screen is active
    When WiFi is disconnected
    Then the status arc should be displayed in red colour
    And this should indicate an error state

  Scenario: Screen navigation from hello world
    Given I am on the hello world screen
    When I rotate the encoder clockwise
    Then the system should transition to the clock screen
    And the transition should be smooth

  Scenario: Button press on hello world screen
    Given I am on the hello world screen
    When I press the rotary encoder button
    Then a button press event should be logged
    And the system should provide appropriate feedback

  Scenario: Return to hello world from last screen
    Given I am on the settings screen (last in sequence)
    When I rotate the encoder clockwise
    Then the system should wrap around to the hello world screen
    And the transition should be seamless
