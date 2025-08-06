Feature: Energy Monitoring Screen
  As a user
  I want to monitor real-time energy consumption and daily peaks
  So that I can track my energy usage patterns

  Background:
    Given the ESP32-S3 Knob device is connected to MQTT
    And EmonTX3 is publishing energy data
    And I am viewing the energy screen

  Scenario: Real-time energy data display
    Given EmonTX3 is sending current energy readings
    When the energy screen is active
    Then I should see the current import power in watts
    And I should see the current solar power generation
    And I should see the net energy balance (import - solar)
    And the values should update in real-time as MQTT messages arrive

  Scenario: Daily peak tracking display
    Given energy data has been collected throughout the day
    When viewing the energy screen
    Then I should see "Today's Peak" value
    And I should see the accumulated kWh for the day
    And the peak value should represent the highest import reading
    And the accumulated kWh should be the sum of all import readings

  Scenario: Energy data formatting
    Given energy values are being displayed
    Then power values should be shown in watts (W)
    And energy values should be shown in kilowatt-hours (kWh)
    And values should be formatted to appropriate decimal places
    And units should be clearly labeled

  Scenario: No energy data available
    Given EmonTX3 is not sending data or MQTT is disconnected
    When the energy screen is displayed
    Then I should see "No data" or "Connecting..." messages
    And the display should indicate the connection status
    And previous values should be retained until new data arrives

  Scenario: Daily reset of peak and accumulation
    Given it's the start of a new day (00:00)
    When the daily reset occurs
    Then the daily peak should reset to 0
    And the daily kWh accumulation should reset to 0
    And the reset should happen automatically
    And a log message should be recorded

  Scenario: MQTT topic handling for energy data
    Given the system is subscribed to emon/emontx3/* topics
    When a message arrives on "emon/emontx3/import"
    Then the import power value should update
    When a message arrives on "emon/emontx3/solar"
    Then the solar power value should update
    When a message arrives on "emon/emontx3/balance"
    Then the balance calculation should update

  Scenario: Energy screen navigation
    Given I am on the energy screen
    When I rotate the encoder clockwise
    Then the system should transition to the weather screen
    When I rotate the encoder counter-clockwise
    Then the system should transition to the clock screen

  Scenario: Button press on energy screen
    Given I am on the energy screen
    When I press the rotary encoder button
    Then the daily peak and accumulation values may reset manually
    Or additional energy statistics may be displayed
    And appropriate feedback should be provided
