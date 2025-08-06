Feature: MQTT Integration and Communication
  As a system administrator
  I want the device to communicate reliably via MQTT
  So that it can be integrated with home automation systems

  Background:
    Given the ESP32-S3 Knob device is connected to WiFi
    And MQTT broker configuration is available

  Scenario: MQTT connection establishment
    Given WiFi is connected
    When the MQTT manager initializes
    Then it should connect to the configured MQTT broker
    And the connection status should be reported as connected
    And all configured topics should be subscribed automatically

  Scenario: MQTT topic subscription
    Given MQTT connection is established
    When the client connects
    Then it should subscribe to the following topics:
      | Topic Category | Topic |
      | Device Control | home/knob/command |
      | Device Control | home/knob/brightness |
      | Device Control | home/knob/haptic |
      | Energy Data | emon/emontx3/balance |
      | Energy Data | emon/emontx3/solar |
      | Energy Data | emon/emontx3/import |
      | Energy Data | emon/emontx3/used |
      | Energy Data | emon/emontx3/tariff |
      | House Info | home/motd |

  Scenario: MQTT message routing to appropriate handlers
    Given MQTT is connected and subscribed to topics
    When a message arrives on "emon/emontx3/import"
    Then it should be routed to the energy data manager
    When a message arrives on "home/knob/brightness"
    Then it should be routed to the device control manager
    When a message arrives on "home/motd"
    Then it should be routed to the MOTD manager

  Scenario: Energy data MQTT message handling
    Given the energy manager is registered for "emon/emontx3" namespace
    When an MQTT message arrives on "emon/emontx3/import" with value "2500"
    Then the current import power should be set to 2500 watts
    And the daily peak should be updated if this is higher
    And the daily accumulation should be incremented
    And the energy screen should reflect the new values

  Scenario: Device control MQTT message handling
    Given the device control manager is registered for "home/knob" namespace
    When an MQTT message arrives on "home/knob/brightness" with value "75"
    Then the screen brightness should be set to 75%
    And a status message should be published to "home/knob/brightness/status"
    When an MQTT message arrives on "home/knob/haptic" with value "on"
    Then haptic feedback should be enabled
    And a status message should be published to "home/knob/haptic/status"

  Scenario: MOTD MQTT message handling
    Given the house screen MOTD handler is registered for "home" namespace
    When an MQTT message arrives on "home/motd" with content "Energy usage today: 24.5 kWh"
    Then the MOTD manager should update the current message
    And the house screen ticker should display the new message
    And the scroll position should reset to the beginning

  Scenario: MQTT connection loss and recovery
    Given MQTT was previously connected
    When the MQTT connection is lost
    Then the connection status should change to disconnected
    And the status arc should turn orange (WiFi only) or red (no WiFi)
    When the connection is restored
    Then all topics should be re-subscribed automatically
    And the status should return to connected (green)

  Scenario: MQTT publish functionality
    Given MQTT is connected
    When the device needs to publish status information
    Then it should publish to appropriate status topics
    And messages should be formatted correctly
    And retained messages should be used for persistent status

  Scenario: MQTT namespace-based message routing
    Given multiple handlers are registered for different namespaces
    When a message arrives on any subscribed topic
    Then the message should be routed to the correct handler based on namespace matching
    And only the appropriate handler should process the message
    And unrecognised topics should be logged but not cause errors

  Scenario: MQTT message validation and error handling
    Given MQTT messages are being received
    When a message contains invalid data format
    Then the system should log the error
    And continue processing other valid messages
    And not crash or become unresponsive
    When a message is received on an unsubscribed topic
    Then it should be ignored gracefully

  Scenario: MQTT auto-reconnection behaviour
    Given MQTT connection is lost
    When the reconnection timer expires
    Then the system should attempt to reconnect automatically
    And should retry with exponential backoff
    And should not block other system operations
    And should restore full functionality when reconnected
