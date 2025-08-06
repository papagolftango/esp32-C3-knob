Feature: Bin Collection Management System
  As a household member
  I want to track bin collection schedules and receive visual reminders
  So that I never miss bin collection days

  Background:
    Given the bin collection manager is initialized
    And the house screen is available

  Scenario: Bin collection manager initialisation
    Given the system starts up
    When the bin collection manager initializes
    Then it should create managers for three bin types:
      | Bin Type | Colour |
      | Black | Dark gray (#404040) |
      | Green | Forest green (#228B22) |
      | Orange | Dark orange (#FF8C00) |
    And each bin should start with NEXT_COLLECTION status
    And default schedule data should be available

  Scenario: Bin status state transitions
    Given a bin is in NEXT_COLLECTION status
    When the schedule indicates preparation is needed tonight
    Then the bin status should change to PREPARE_TONIGHT
    Given a bin is in PREPARE_TONIGHT status
    When the collection day arrives
    Then the bin status should change to DUE_TODAY
    Given a bin is in DUE_TODAY status
    When the collection day passes
    Then the bin status should return to NEXT_COLLECTION

  Scenario: Visual display for NEXT_COLLECTION status
    Given a bin is in NEXT_COLLECTION status
    When the house screen is displayed
    Then the bin icon should be visible with appropriate colour
    And the icon should be displayed at normal opacity (100%)
    And no special animations should be active
    And no additional icons should be displayed

  Scenario: Visual display for PREPARE_TONIGHT status
    Given a bin is in PREPARE_TONIGHT status
    When the house screen is displayed
    Then the bin icon should be visible with appropriate colour
    And the icon should flash with alternating opacity (100% ↔ 30%)
    And the flash rate should be 500ms intervals
    And this should serve as a clear preparation reminder

  Scenario: Visual display for DUE_TODAY status
    Given a bin is in DUE_TODAY status
    When the house screen is displayed
    Then the bin icon should be visible with appropriate colour
    And the icon should be displayed at normal opacity
    And a golden lorry icon (🚛) should also be displayed
    And the lorry indicates that collection is happening today

  Scenario: Multiple bins with different statuses
    Given the black bin is PREPARE_TONIGHT
    And the green bin is DUE_TODAY
    And the orange bin is NEXT_COLLECTION
    When the house screen displays the status
    Then the black bin should flash at 500ms intervals
    And the green bin should be solid with lorry icon
    And the orange bin should be solid without special effects
    And all three should be displayed simultaneously

  Scenario: Flash timer management
    Given one or more bins are in PREPARE_TONIGHT status
    When the house screen is active
    Then a flash timer should be created and active
    And the timer should run every 500ms
    Given no bins are in PREPARE_TONIGHT status
    When the statuses are updated
    Then the flash timer should be deleted/stopped
    And no flashing animations should be visible

  Scenario: Bin collection status text display
    Given bins have various statuses
    When the house screen displays the collection information
    Then appropriate status text should be shown below the icons
    And the text should summarize the current collection situation
    And it should be informative and concise

  Scenario: Schedule update interface
    Given external schedule data is available
    When the schedule update function is called
    Then it should accept collection day information
    And update bin statuses based on current date/time
    And trigger visual display updates
    And log the schedule changes appropriately

  Scenario: Data change detection
    Given the bin collection display is active
    When any bin status changes
    Then the hasDataChanged() function should return true
    And the visual display should update automatically
    And the change flag should be cleared after UI update

  Scenario: Bin collection manager error handling
    Given the bin collection system is running
    When invalid schedule data is provided
    Then the system should maintain current statuses
    And log appropriate error messages
    And not crash or become unresponsive
    When no schedule data is available
    Then default "Loading schedule..." message should be displayed

  Scenario: Integration with MQTT for future schedule updates
    Given MQTT connectivity is available
    When bin collection schedule data arrives via MQTT
    Then the bin collection manager should be updated
    And visual displays should reflect the new schedule
    And the system should handle real-time schedule changes
    And maintain schedule persistence across restarts
