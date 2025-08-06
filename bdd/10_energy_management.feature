Feature: Energy Data Management and Peak Tracking
  As an energy-conscious user
  I want to track real-time energy consumption and daily peak usage
  So that I can monitor and optimise my energy consumption patterns

  Background:
    Given the energy data manager is initialized
    And MQTT connection is established
    And EmonTX3 is configured to send energy data

  Scenario: Energy data manager initialisation
    Given the system starts up
    When the energy data manager initializes
    Then it should register as a handler for "emon/emontx3" namespace
    And all energy values should be initialized to 0.0
    And daily peak should be reset to 0.0
    And daily kWh accumulation should be reset to 0.0
    And the last reset timestamp should be set to current day

  Scenario: Real-time import power processing
    Given the energy manager is active
    When an MQTT message arrives on "emon/emontx3/import" with value "2500.5"
    Then the current import power should be set to 2500.5 watts
    And if 2500.5 > current daily peak, the daily peak should be updated
    And the kWh accumulation should be incremented based on time elapsed
    And the energy screen should display the updated values

  Scenario: Daily peak tracking behaviour
    Given the current daily peak is 2000 watts
    When import power of 2500 watts is received
    Then the daily peak should be updated to 2500 watts
    When import power of 1800 watts is received
    Then the daily peak should remain at 2500 watts
    And the current import should show 1800 watts

  Scenario: Daily kWh accumulation calculation
    Given the last energy reading was 2000 watts at time T1
    When a new reading of 2400 watts arrives at time T2
    Then the kWh accumulation should increase by:
      (2000 + 2400) / 2 * (T2 - T1) / 3600 / 1000
    And the calculation should handle varying time intervals correctly
    And accumulated energy should continuously increase throughout the day

  Scenario: Solar power data processing
    Given the energy manager is active
    When an MQTT message arrives on "emon/emontx3/solar" with value "1500.0"
    Then the current solar power should be set to 1500.0 watts
    And the energy balance calculation should be updated
    And the energy screen should display the solar generation

  Scenario: Energy balance calculation
    Given current import power is 2500 watts
    And current solar power is 1500 watts
    When the balance is calculated
    Then the net energy usage should be 1000 watts (2500 - 1500)
    And this should represent the actual consumption from the grid
    And the balance should update whenever import or solar values change

  Scenario: Daily reset at midnight
    Given it's 23:59:59 on any day
    And daily peak is 3200 watts
    And daily accumulation is 45.6 kWh
    When the clock advances to 00:00:00 of the next day
    Then the daily peak should be reset to 0.0 watts
    And the daily kWh accumulation should be reset to 0.0 kWh
    And a log message should confirm the daily reset
    And the current import/solar values should be maintained

  Scenario: Handling missing or invalid energy data
    Given the energy manager is receiving data
    When an MQTT message arrives with invalid/non-numeric value
    Then the invalid data should be ignored
    And previous valid values should be retained
    And an error should be logged appropriately
    When no energy data is received for extended period
    Then the display should indicate "No data" or "Connecting..."

  Scenario: Energy data persistence across daily resets
    Given energy data has been collected throughout the day
    When the daily reset occurs at midnight
    Then the historical daily peak should be available if needed
    And the system should be ready to track the new day's data
    And all current readings should continue to be processed normally

  Scenario: MQTT topic routing for energy data
    Given the energy manager is subscribed to energy topics
    When messages arrive on different energy topics:
      | Topic | Value | Expected Result |
      | emon/emontx3/import | 2200 | Import power = 2200W |
      | emon/emontx3/solar | 800 | Solar power = 800W |
      | emon/emontx3/balance | 1400 | Balance = 1400W |
      | emon/emontx3/used | 1900 | Used power = 1900W |
      | emon/emontx3/tariff | 0.28 | Tariff rate = £0.28/kWh |
    Then each value should be processed according to its topic type

  Scenario: Energy screen data updates
    Given the energy screen is currently displayed
    When energy data values change via MQTT
    Then the display should update automatically
    And show current import, solar, and balance in real-time
    And display today's peak and accumulated kWh
    And all values should be formatted appropriately (W, kWh, etc.)

  Scenario: Energy manager error recovery
    Given the energy manager is running normally
    When MQTT connection is lost and restored
    Then the manager should continue processing new data
    And accumulated values should not be lost during disconnection
    And peak tracking should resume correctly
    And daily reset timing should not be affected
