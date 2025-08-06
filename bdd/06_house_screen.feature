Feature: House Management Screen
  As a user
  I want to manage household information including bin collection and view news updates
  So that I can stay organised and informed about household tasks

  Background:
    Given the ESP32-S3 Knob device is connected to MQTT
    And I am viewing the house screen

  Scenario: House screen initial display
    Given the house screen is loaded
    When the screen initializes
    Then I should see "🏠 BIN COLLECTION" as the title
    And I should see a bin icons container in the center
    And I should see a scrolling MOTD ticker at the bottom
    And I should see "Loading schedule..." initially

  Scenario: Bin collection visual indicators
    Given bin collection data is available
    When bins are in different states
    Then black bins should be displayed in dark grey colour
    And green bins should be displayed in forest green colour
    And orange bins should be displayed in dark orange colour
    And only relevant bins should be visible based on their status

  Scenario: Bin collection state - Next Collection
    Given a bin is in NEXT_COLLECTION status
    When the house screen is displayed
    Then the corresponding bin icon should be visible
    And the bin should be displayed at normal opacity
    And no special animations should be active

  Scenario: Bin collection state - Prepare Tonight
    Given a bin is in PREPARE_TONIGHT status
    When the house screen is displayed
    Then the corresponding bin icon should be visible
    And the bin icon should flash at 500ms intervals
    And the opacity should alternate between 100% and 30%
    And this should provide a clear preparation reminder

  Scenario: Bin collection state - Due Today
    Given a bin is in DUE_TODAY status
    When the house screen is displayed
    Then the corresponding bin icon should be visible
    And a golden lorry icon (🚛) should also be displayed
    And the lorry indicates collection is happening today

  Scenario: Multiple bins with different states
    Given multiple bins have different statuses
    When the house screen displays the status
    Then each bin should display according to its individual status
    And flashing should only apply to PREPARE_TONIGHT bins
    And the lorry should appear if any bin is DUE_TODAY
    And the display should handle all combinations correctly

  Scenario: MOTD ticker display and scrolling
    Given the MOTD ticker is initialized
    When a message is displayed
    Then the text should appear in green colour
    And if the text fits in the container, it should be centered
    And if the text is longer than the container, it should scroll
    And scrolling should move at 2 pixels per 80ms tick
    And scrolling should reset when text completely passes

  Scenario: MOTD message updates via MQTT
    Given the house screen is active
    When a new message is published to "home/motd" topic
    Then the ticker should update with the new message
    And the scroll position should reset to the beginning
    And the previous message should be replaced
    And a log message should confirm the update

  Scenario: MOTD default message
    Given no MOTD message has been received
    When the house screen is displayed
    Then the default message should be shown:
      "Welcome to Smart Home • Bin Collection Status • Energy Monitoring Active"

  Scenario: Independent timing systems
    Given both bin flashing and MOTD scrolling are active
    When observing the animations
    Then bin icons should flash every 500ms
    And MOTD text should scroll every 80ms
    And these timers should operate independently
    And neither should interfere with the other

  Scenario: House screen navigation
    Given I am on the house screen
    When I rotate the encoder clockwise
    Then the system should transition to the settings screen
    When I rotate the encoder counter-clockwise
    Then the system should transition to the weather screen

  Scenario: Button press on house screen
    Given I am on the house screen
    When I press the rotary encoder button
    Then the bin collection display should refresh
    And the MOTD display should update if changed
    And appropriate feedback should be logged

  Scenario: Data synchronisation and updates
    Given the house screen is active
    When bin collection data changes
    Then the visual display should update automatically
    When MOTD data changes
    Then the ticker should update automatically
    And both updates should happen in real-time
