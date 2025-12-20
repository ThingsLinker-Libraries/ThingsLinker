/**
 * @file TL_MQTT.h
 * @brief MQTT communication - publish and subscribe to widgets
 *
 * This file handles ALL MQTT operations:
 * - Connect to MQTT server
 * - Subscribe to control topics
 * - Publish sensor data
 * - Auto-reconnect
 *
 * Super simple to use!
 */

#ifndef TL_MQTT_H
#define TL_MQTT_H

#include <Arduino.h>

/**
 * @brief Connect to MQTT server
 * @param authToken Device authentication token
 * @param blueprintId Device blueprint ID
 * @param clientKey MQTT client key
 * @param secretKey MQTT secret key
 * @return true if connected
 *
 * Example:
 *   connectMQTT("EiAbhe...", "BLUEZ8hnUqddtfu5", "client-xxx", "secret-xxx");
 */
bool connectMQTT(const char* authToken, const char* blueprintId, const char* clientKey, const char* secretKey);

/**
 * @brief Disconnect from MQTT
 *
 * Example:
 *   disconnectMQTT();
 */
void disconnectMQTT();

/**
 * @brief Check if MQTT is connected
 * @return true if connected
 *
 * Example:
 *   if (isMQTTConnected()) {
 *     Serial.println("MQTT Online!");
 *   }
 */
bool isMQTTConnected();

/**
 * @brief Subscribe to a pin (to receive control from app)
 * @param widgetType Widget type (Button, Switch, Slider, Gauge, etc.)
 * @param pin Virtual pin (V0 to V124 supported - 125 pins total)
 * @param callback Function to call when value received
 *
 * Example:
 *   subscribeMQTT("Button", "V0", [](float value) {
 *     Serial.println("Button: " + String(value));
 *   });
 */
void subscribeMQTT(const char* widgetType, const char* pin, void (*callback)(float value));

/**
 * @brief Subscribe to button widget (receives bool value)
 * @param widgetType Widget type (usually "Button")
 * @param pin Virtual pin (V0 to V124)
 * @param callback Function to call with bool value
 *
 * Example:
 *   subscribeMQTTButton("Button", "V0", [](bool pressed) {
 *     digitalWrite(LED_PIN, pressed ? HIGH : LOW);
 *   });
 */
void subscribeMQTTButton(const char* widgetType, const char* pin, void (*callback)(bool value));

/**
 * @brief Publish value to a pin (send sensor data to app)
 * @param widgetType Widget type (Button, Switch, Slider, Gauge, etc.)
 * @param pin Virtual pin (V0 to V124 supported - 125 pins total)
 * @param value Value to send
 *
 * Example:
 *   publishMQTT("Gauge", "V1", 25.5);    // Temperature
 *   publishMQTT("LED", "V2", 1);         // LED ON
 */
void publishMQTT(const char* widgetType, const char* pin, float value);

/**
 * @brief Process MQTT messages (call in loop)
 *
 * Example:
 *   void loop() {
 *     loopMQTT();
 *   }
 */
void loopMQTT();

/**
 * @brief Get device chip ID
 * @return Chip ID as hex string
 *
 * Example:
 *   String id = getChipID();
 *   Serial.println("Chip ID: " + id);
 */
String getChipID();

#endif // TL_MQTT_H
