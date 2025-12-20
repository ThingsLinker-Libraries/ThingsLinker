/**
 * @file TL_MQTT.cpp
 * @brief Implementation of MQTT communication
 */

#include "TL_MQTT.h"
#include "TL_Config.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

// Global variables
static WiFiClient wifiClient;
static PubSubClient mqttClient(wifiClient);
static String authToken = "";
static String blueprintId = "";
static unsigned long lastReconnect = 0;
// Status management is now handled by MQTT Last Will and Testament (LWT)

// Pin callbacks (V0 - V124 = 125 pins total)
struct PinCallback
{
  String pin;
  String widgetType;
  void (*callback)(float);
};
static PinCallback pinCallbacks[125];
static int pinCallbackCount = 0;

// Button callbacks (for bool conversion)
struct ButtonCallback
{
  String pin;
  String widgetType;
  void (*callback)(bool);
};
static ButtonCallback buttonCallbacks[125];
static int buttonCallbackCount = 0;

/**
 * @brief MQTT callback for incoming messages
 */
static void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  // Convert payload to string
  String message = "";
  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.println("[MQTT] Message: " + String(topic));
  Serial.println("[MQTT] Data: " + message);

  // Parse JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error)
  {
    Serial.println("[MQTT] JSON parse error");
    return;
  }

  if (!doc.containsKey("v"))
  {
    Serial.println("[MQTT] No 'v' field");
    return;
  }

  float value = doc["v"];

  // Extract pin from topic: device/{WidgetType}/{BlueprintId}/{AuthToken}/{VirtualPin}/
  String topicStr = String(topic);

  // Find all slash positions
  int slashPos[5] = {-1, -1, -1, -1, -1};
  int slashCount = 0;

  for (unsigned int i = 0; i < topicStr.length() && slashCount < 5; i++)
  {
    if (topicStr.charAt(i) == '/')
    {
      slashPos[slashCount] = i;
      slashCount++;
    }
  }

  String pin = "";
  // Pin is between 4th and 5th slash (index 3 and 4)
  if (slashCount >= 5)
  {
    pin = topicStr.substring(slashPos[3] + 1, slashPos[4]);
  }

  Serial.println("[MQTT] Pin: " + pin + ", Value: " + String(value));

  // Check button callbacks first (for bool conversion)
  for (int i = 0; i < buttonCallbackCount; i++)
  {
    if (buttonCallbacks[i].pin == pin && buttonCallbacks[i].callback)
    {
      buttonCallbacks[i].callback(value > 0);
      return;
    }
  }

  // Find regular float callback
  for (int i = 0; i < pinCallbackCount; i++)
  {
    if (pinCallbacks[i].pin == pin && pinCallbacks[i].callback)
    {
      pinCallbacks[i].callback(value);
      break;
    }
  }
}

/**
 * @brief Connect to MQTT
 */
bool connectMQTT(const char *authTokenParam, const char *blueprintIdParam, const char *clientKey, const char *secretKey)
{
  authToken = String(authTokenParam);
  blueprintId = String(blueprintIdParam);

  Serial.println("[MQTT] Connecting to " + String(MQTT_SERVER));

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setKeepAlive(MQTT_KEEPALIVE);
  mqttClient.setBufferSize(MQTT_MAX_PACKET_SIZE); // CRITICAL: Required for LWT with long credentials
  Serial.println("[MQTT] Buffer size: " + String(MQTT_MAX_PACKET_SIZE) + " bytes");

  // Use client ID extracted from clientKey (first 40 chars before '-')
  // This matches Flutter's approach and may be required by broker
  String clientId = "ThingsLinker_" + getChipID();

  Serial.println("[MQTT] Client ID: " + clientId);

  // Prepare status topic: device/status/{BlueprintId}/{AuthToken}/
  String statusTopic = "device/status/" + blueprintId + "/" + authToken + "/";
  Serial.println("[MQTT] Status topic: " + statusTopic);

  // Connect with Last Will and Testament (LWT)
  // When device disconnects unexpectedly, broker will publish "OFFLINE" automatically
  bool connected;
  if (strlen(clientKey) > 0 && strlen(secretKey) > 0)
  {
    // Connect with authentication + LWT (production)
    Serial.println("[MQTT] Connecting with authentication + LWT...");
    connected = mqttClient.connect(
      clientId.c_str(),           // Client ID
      clientKey,                  // Username
      secretKey,                  // Password
      statusTopic.c_str(),        // Will topic
      0,                          // Will QoS (1 = At least once) - Changed from 2 for broker compatibility
      true,                       // Will retain (true = retained message)
      "OFFLINE"                   // Will message (published when device disconnects)
    );
  }
  else
  {
    // Connect without authentication + LWT (testing with HiveMQ)
    Serial.println("[MQTT] Connecting without authentication (test mode) + LWT...");
    connected = mqttClient.connect(
        clientId.c_str(),    // Client ID
        statusTopic.c_str(), // Will topic
        0,                   // Will QoS (1 = At least once) - Changed from 2 for broker compatibility
        true,                // Will retain
        "OFFLINE"            // Will message
    );
  }

  if (connected)
  {
    Serial.println("[MQTT] ✓ Connected!");

    // Immediately publish ONLINE status (retained)
    // This replaces the LWT message until device disconnects
    mqttClient.publish(statusTopic.c_str(), "ONLINE", true);
    Serial.println("[MQTT] ✓ Published ONLINE status");

    return true;
  }
  else
  {
    Serial.println("[MQTT] ✗ Connection failed");
    int state = mqttClient.state();
    Serial.println("[MQTT] State: " + String(state));

    // Detailed error description
    switch (state)
    {
    case -4:
      Serial.println("[MQTT] Error: Connection timeout - Server didn't respond");
      break;
    case -3:
      Serial.println("[MQTT] Error: Connection lost - Network broken");
      break;
    case -2:
      Serial.println("[MQTT] Error: Connect failed - Network unreachable");
      break;
    case -1:
      Serial.println("[MQTT] Error: AUTHENTICATION FAILED - Wrong username/password");
      Serial.println("[MQTT] → Check if MQTT broker is configured with these credentials");
      Serial.println("[MQTT] → Or broker may not be running at " + String(MQTT_SERVER));
      break;
    case 1:
      Serial.println("[MQTT] Error: Bad protocol version");
      break;
    case 2:
      Serial.println("[MQTT] Error: Client ID rejected");
      break;
    case 3:
      Serial.println("[MQTT] Error: Server unavailable");
      break;
    case 4:
      Serial.println("[MQTT] Error: Bad username or password");
      break;
    case 5:
      Serial.println("[MQTT] Error: Not authorized");
      break;
    default:
      Serial.println("[MQTT] Error: Unknown error code");
    }

    return false;
  }
}

/**
 * @brief Disconnect from MQTT
 */
void disconnectMQTT()
{
  if (mqttClient.connected())
  {
    mqttClient.disconnect();
    Serial.println("[MQTT] Disconnected");
  }
}

/**
 * @brief Check if MQTT is connected
 */
bool isMQTTConnected()
{
  return mqttClient.connected();
}

/**
 * @brief Subscribe to a pin
 */
void subscribeMQTT(const char *widgetType, const char *pin, void (*callback)(float value))
{
  if (pinCallbackCount >= 125)
  {
    Serial.println("[MQTT] Max pins reached (125 max)");
    return;
  }

  // Save callback
  pinCallbacks[pinCallbackCount].pin = String(pin);
  pinCallbacks[pinCallbackCount].widgetType = String(widgetType);
  pinCallbacks[pinCallbackCount].callback = callback;
  pinCallbackCount++;

  // Subscribe to topic: device/{WidgetType}/{BlueprintId}/{AuthToken}/{VirtualPin}/
  if (mqttClient.connected())
  {
    String topic = "device/" + String(widgetType) + "/" + blueprintId + "/" + authToken + "/" + String(pin) + "/";
    bool success = mqttClient.subscribe(topic.c_str(), 1);

    if (success)
    {
      Serial.println("[MQTT] ✓ Subscribed: " + topic);
    }
    else
    {
      Serial.println("[MQTT] ✗ Subscribe failed: " + topic);
    }
  }
}

/**
 * @brief Subscribe to button widget (bool callback)
 */
void subscribeMQTTButton(const char *widgetType, const char *pin, void (*callback)(bool value))
{
  if (buttonCallbackCount >= 125)
  {
    Serial.println("[MQTT] Max button pins reached (125 max)");
    return;
  }

  // Save button callback
  buttonCallbacks[buttonCallbackCount].pin = String(pin);
  buttonCallbacks[buttonCallbackCount].widgetType = String(widgetType);
  buttonCallbacks[buttonCallbackCount].callback = callback;
  buttonCallbackCount++;

  // Subscribe to topic: device/{WidgetType}/{BlueprintId}/{AuthToken}/{VirtualPin}/
  if (mqttClient.connected())
  {
    String topic = "device/" + String(widgetType) + "/" + blueprintId + "/" + authToken + "/" + String(pin) + "/";
    bool success = mqttClient.subscribe(topic.c_str(), 1);

    if (success)
    {
      Serial.println("[MQTT] ✓ Subscribed (Button): " + topic);
    }
    else
    {
      Serial.println("[MQTT] ✗ Subscribe failed: " + topic);
    }
  }
}

/**
 * @brief Publish value to a pin
 */
void publishMQTT(const char *widgetType, const char *pin, float value)
{
  if (!mqttClient.connected())
  {
    // Silently fail - reconnection is handled by checkConnections()
    return;
  }

  // Build topic: device/{WidgetType}/{BlueprintId}/{AuthToken}/{VirtualPin}/
  String topic = "device/" + String(widgetType) + "/" + blueprintId + "/" + authToken + "/" + String(pin) + "/";

  // Build payload
  StaticJsonDocument<128> doc;
  doc["v"] = value;
  doc["t"] = millis() / 1000;

  String payload;
  serializeJson(doc, payload);

  // Publish
  bool success = mqttClient.publish(topic.c_str(), payload.c_str(), false);

  if (success)
  {
    Serial.println("[MQTT] ✓ Published " + String(pin) + ": " + String(value));
  }
  else
  {
    Serial.println("[MQTT] ✗ Publish failed");
  }
}

/**
 * @brief Process MQTT messages
 */
void loopMQTT()
{
  if (mqttClient.connected())
  {
    mqttClient.loop();
  }
  // Note: Reconnection is handled by ThingsLinker::checkConnections()
  // Online/Offline status is managed by MQTT LWT (Last Will and Testament)
}

/**
 * @brief Get chip ID
 */
String getChipID()
{
#ifdef ESP32
  uint64_t chipid = ESP.getEfuseMac();
  char chipIdStr[13];
  sprintf(chipIdStr, "%04X%08X", (uint16_t)(chipid >> 32), (uint32_t)chipid);
  return String(chipIdStr);
#elif defined(ESP8266)
  return String(ESP.getChipId(), HEX);
#else
  return "UNKNOWN";
#endif
}
