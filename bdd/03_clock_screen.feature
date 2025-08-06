Feature: Clock Screen
  As a user
  I want to view the current time and date
  So that I can stay informed about the time

  Background:
    Given the ESP32-S3 Knob device is initialized
    And I have navigated to the clock screen

  Scenario: Clock display with valid time
    Given the system has a valid time source (NTP or manual)
    When the clock screen is displayed
    Then I should see the current time in HH:MM format
    And I should see the current date
    And the time should update every minute
    And the display should use a readable font

  Scenario: Clock display without time synchronisation
    Given the system has not synchronised time yet
    When the clock screen is displayed
    Then I should see a placeholder or "Time syncing..." message
    And the display should indicate that time is not available

  Scenario: Time format display
    Given the clock screen is active
    When displaying time
    Then the time should be shown in 24-hour format
    And the date should include day, month, and year
    And the format should be consistent and readable

  Scenario: Clock screen navigation
    Given I am on the clock screen
    When I rotate the encoder clockwise
    Then the system should transition to the energy screen
    When I rotate the encoder counter-clockwise
    Then the system should transition to the hello world screen

  Scenario: Button press on clock screen
    Given I am on the clock screen
    When I press the rotary encoder button
    Then a clock button press event should be logged
    And the system may trigger time synchronisation
    Or display additional time information

  Scenario: Automatic time updates
    Given the clock screen is displayed
    And the system has valid time
    When one minute passes
    Then the displayed time should automatically update
    And no user interaction should be required
    And the update should be smooth without flickering

  Scenario: Time zone handling
    Given the clock screen is active
    When the system is configured with a specific timezone
    Then the displayed time should reflect the local timezone
    And daylight saving time adjustments should be automatic
