Feature: Weather Screen
  As a user
  I want to view current weather information
  So that I can stay informed about weather conditions

  Background:
    Given the ESP32-S3 Knob device is initialized
    And I have navigated to the weather screen

  Scenario: Weather screen placeholder display
    Given the weather screen is loaded
    When no weather data is available
    Then I should see "Weather Info" as the title
    And I should see placeholder text indicating weather data will be available
    And the screen should be formatted consistently with other screens

  Scenario: Weather data integration readiness
    Given the weather screen infrastructure is in place
    When weather API integration is implemented
    Then the screen should be able to display:
      | Data Type | Format |
      | Temperature | Celsius with ° symbol |
      | Humidity | Percentage with % symbol |
      | Weather condition | Text description |
      | Location | City name |
    And the layout should accommodate all weather elements

  Scenario: Weather screen navigation
    Given I am on the weather screen
    When I rotate the encoder clockwise
    Then the system should transition to the house screen
    When I rotate the encoder counter-clockwise
    Then the system should transition to the energy screen

  Scenario: Button press on weather screen
    Given I am on the weather screen
    When I press the rotary encoder button
    Then a weather button press event should be logged
    And the system may trigger a weather data refresh
    Or display additional weather details

  Scenario: Weather data error handling
    Given weather API integration is implemented
    When weather data cannot be retrieved
    Then the screen should display "Weather unavailable"
    And should show the last known data if available
    And indicate when the data was last updated

  Scenario: Weather data formatting
    Given weather data is available
    When displaying weather information
    Then temperature should be rounded to whole degrees
    And humidity should be shown as percentage
    And weather descriptions should be concise
    And all text should be readable on the circular display

  Scenario: Automatic weather updates
    Given weather data is being displayed
    When the weather update interval expires
    Then the system should automatically fetch new data
    And update the display with fresh information
    And handle any network errors gracefully
