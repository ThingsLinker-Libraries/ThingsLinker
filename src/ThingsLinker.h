/**
 * @file ThingsLinker.h
 * @brief ThingsLinker IoT Library - Super Simple!
 *
 * ==================================================
 * EASIEST IOT LIBRARY FOR ESP32!
 * ==================================================
 *
 * Just 3 steps:
 * 1. Create: ThingsLinker iot("device_token");
 * 2. Setup: iot.begin("client_key", "secret_key");
 * 3. Loop: iot.run();
 *
 * That's it! Library handles everything automatically:
 * ✓ BLE provisioning (if no WiFi)
 * ✓ WiFi connection
 * ✓ MQTT connection
 * ✓ Auto-reconnect
 *
 * ==================================================
 */

#ifndef THINGSLINKER_H
#define THINGSLINKER_H

#include <Arduino.h>
#include "TL_Config.h"
#include "TL_BLE.h"
#include "TL_WiFi.h"
#include "TL_MQTT.h"
#include "TL_Base64.h"
#include "TL_Storage.h"

class ThingsLinker {
public:
  /**
   * @brief Create ThingsLinker
   * @param authToken Your device authentication token
   * @param blueprintId Your device blueprint ID (required for OTA)
   *
   * Example:
   *   ThingsLinker iot("EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s", "BLUEZ8hnUqddtfu5");
   */
  ThingsLinker(const char* authToken, const char* blueprintId);

  /**
   * @brief Start ThingsLinker (call in setup)
   * @param clientKey MQTT client key
   * @param secretKey MQTT secret key
   *
   * Example:
   *   iot.begin("client-xxx", "secret-xxx");
   */
  void begin(const char* clientKey, const char* secretKey);

  /**
   * @brief Run ThingsLinker (call in loop)
   *
   * Example:
   *   void loop() {
   *     iot.run();
   *   }
   */
  void run();

  // ========== Easy Widget Functions ==========
  // Supports V0 to V124 (125 pins total!)

  /**
   * @brief Control button (send 0 or 1 to app)
   * @param pin Virtual pin V0-V124
   */
  void button(const char* pin, bool value);

  /**
   * @brief Control LED (send 0 or 1 to app)
   * @param pin Virtual pin V0-V124
   */
  void led(const char* pin, bool value);

  /**
   * @brief Send gauge value (temperature, humidity, etc.)
   * @param pin Virtual pin V0-V124
   */
  void gauge(const char* pin, float value);

  /**
   * @brief Send slider value (0-100)
   * @param pin Virtual pin V0-V124
   */
  void slider(const char* pin, float value);

  /**
   * @brief Send any value
   * @param pin Virtual pin V0-V124
   */
  void send(const char* pin, float value);

  /**
   * @brief Listen to button from app
   * @param pin Virtual pin V0-V124
   *
   * Example:
   *   iot.onButton("V0", [](bool pressed) {
   *     if (pressed) {
   *       digitalWrite(LED_PIN, HIGH);
   *     }
   *   });
   */
  void onButton(const char* pin, void (*callback)(bool value));

  /**
   * @brief Listen to slider from app
   * @param pin Virtual pin V0-V124
   *
   * Example:
   *   iot.onSlider("V1", [](float value) {
   *     analogWrite(PWM_PIN, (int)value);
   *   });
   */
  void onSlider(const char* pin, void (*callback)(float value));

  /**
   * @brief Listen to any value from app
   * @param pin Virtual pin V0-V124
   *
   * Example:
   *   iot.onValue("V2", [](float value) {
   *     Serial.println("Received: " + String(value));
   *   });
   */
  void onValue(const char* pin, void (*callback)(float value));

  // ========== Status Functions ==========

  /**
   * @brief Check if WiFi is connected
   */
  bool wifiConnected();

  /**
   * @brief Check if MQTT is connected
   */
  bool mqttConnected();

  /**
   * @brief Check if BLE is active
   */
  bool bleActive();

  /**
   * @brief Get WiFi IP address
   */
  String getIP();

  /**
   * @brief Get chip ID
   */
  String getChipID();

  // ========== Storage Functions (Preferences) ==========

  /**
   * @brief Save string data from app
   *
   * Example:
   *   iot.saveString("deviceName", "Living Room");
   *   iot.saveString("location", "Building A");
   */
  bool saveString(const char* key, const String& value);

  /**
   * @brief Get saved string data
   *
   * Example:
   *   String name = iot.getString("deviceName", "Unnamed");
   */
  String getString(const char* key, const String& defaultValue = "");

  /**
   * @brief Save integer data from app
   *
   * Example:
   *   iot.saveInt("threshold", 25);
   */
  bool saveInt(const char* key, int value);

  /**
   * @brief Get saved integer data
   *
   * Example:
   *   int threshold = iot.getInt("threshold", 20);
   */
  int getInt(const char* key, int defaultValue = 0);

  /**
   * @brief Save float data from app
   *
   * Example:
   *   iot.saveFloat("temperature", 25.5);
   */
  bool saveFloat(const char* key, float value);

  /**
   * @brief Get saved float data
   *
   * Example:
   *   float temp = iot.getFloat("temperature", 0.0);
   */
  float getFloat(const char* key, float defaultValue = 0.0);

  /**
   * @brief Save boolean data from app
   *
   * Example:
   *   iot.saveBool("alarmActive", true);
   */
  bool saveBool(const char* key, bool value);

  /**
   * @brief Get saved boolean data
   *
   * Example:
   *   bool active = iot.getBool("alarmActive", false);
   */
  bool getBool(const char* key, bool defaultValue = false);

  /**
   * @brief Check if key exists in storage
   *
   * Example:
   *   if (iot.hasKey("deviceName")) {
   *     Serial.println("Name is set!");
   *   }
   */
  bool hasKey(const char* key);

  /**
   * @brief Remove a key from storage
   *
   * Example:
   *   iot.removeKey("oldData");
   */
  bool removeKey(const char* key);

  /**
   * @brief Clear all stored data (except WiFi)
   *
   * Example:
   *   iot.clearAllData();
   */
  void clearAllData();

  // ========== Advanced Functions ==========

  /**
   * @brief Reset WiFi (start BLE provisioning again)
   *
   * Example:
   *   iot.resetWiFi();  // Clear WiFi and restart BLE
   */
  void resetWiFi();

  /**
   * @brief Enable/disable debug messages
   */
  void debug(bool enable);

private:
  const char* _authToken;
  const char* _blueprintId;
  const char* _clientKey;
  const char* _secretKey;
  bool _debugEnabled;
  bool _initialized;
  static bool _shouldRestart;
  unsigned long _lastCheck;

  void handleBLEProvisioning();
  void checkConnections();
  String buildTopic(const char* widgetType, const char* pin);
};

#endif // THINGSLINKER_H
