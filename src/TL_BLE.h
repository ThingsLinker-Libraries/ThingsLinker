/**
 * @file TL_BLE.h
 * @brief BLE provisioning - handles WiFi setup via mobile app
 *
 * This file handles ALL BLE operations:
 * - Start BLE advertising
 * - Receive WiFi credentials from app
 * - Stop BLE after WiFi connects
 *
 * Super simple to use!
 */

#ifndef TL_BLE_H
#define TL_BLE_H

#include <Arduino.h>

#ifdef ESP32
  #include <BLEDevice.h>
  #include <BLEServer.h>
  #include <BLEUtils.h>
  #include <BLE2902.h>
#endif

/**
 * @brief Start BLE provisioning
 * @param deviceName Name shown in app (e.g., "ThingsLinker_ABC123")
 * @return true if BLE started successfully
 *
 * Example:
 *   startBLE("MyDevice");
 */
bool startBLE(const char* deviceName);

/**
 * @brief Stop BLE provisioning
 *
 * Example:
 *   stopBLE();
 */
void stopBLE();

/**
 * @brief Check if BLE is active
 * @return true if BLE is running
 *
 * Example:
 *   if (isBLEActive()) {
 *     Serial.println("BLE is ON");
 *   }
 */
bool isBLEActive();

/**
 * @brief Set callback when WiFi credentials are received
 * @param callback Function to call with ssid and password
 *
 * Example:
 *   onBLECredentialsReceived([](String ssid, String password) {
 *     Serial.println("Got WiFi: " + ssid);
 *   });
 */
void onBLECredentialsReceived(void (*callback)(String ssid, String password));

/**
 * @brief Send status back to mobile app
 * @param connected true if WiFi connected
 * @param ip IP address (if connected)
 *
 * Example:
 *   sendBLEStatus(true, "192.168.1.100");
 */
void sendBLEStatus(bool connected, const String& ip);

/**
 * @brief Set callback when BLE client disconnects
 * @param callback Function to call when app disconnects
 *
 * Example:
 *   onBLEDisconnected([]() {
 *     Serial.println("App disconnected, connecting MQTT...");
 *   });
 */
void onBLEDisconnected(void (*callback)());

/**
 * @brief Set callback when onboarding confirmation received from app
 * @param callback Function to call when app confirms successful registration
 *
 * Example:
 *   onBLEOnboardingComplete([]() {
 *     Serial.println("Backend registration complete, restarting...");
 *     ESP.restart();
 *   });
 */
void onBLEOnboardingComplete(void (*callback)());

#endif // TL_BLE_H
