/**
 * @file TL_WiFi.cpp
 * @brief Implementation of WiFi management
 */

#include "TL_WiFi.h"
#include "TL_Config.h"

#ifdef ESP32
  #include <WiFi.h>
  #include <Preferences.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <EEPROM.h>
#endif

// Preferences for ESP32
#ifdef ESP32
static Preferences preferences;
#endif

/**
 * @brief Connect to WiFi with credentials
 */
bool connectWiFi(const char* ssid, const char* password, bool saveCredentials) {
  Serial.println("[WiFi] Connecting to: " + String(ssid));

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_CONNECT_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[WiFi] ✓ Connected!");
    Serial.println("[WiFi] IP: " + WiFi.localIP().toString());
    Serial.println("[WiFi] Signal: " + String(WiFi.RSSI()) + " dBm");

    if (saveCredentials) {
      #ifdef ESP32
        preferences.begin("thingslinker", false);
        preferences.putString("ssid", ssid);
        preferences.putString("password", password);
        preferences.putBool("saved", true);
        preferences.end();
        Serial.println("[WiFi] ✓ Credentials saved");
      #endif
    }

    return true;
  } else {
    Serial.println("[WiFi] ✗ Connection failed");
    return false;
  }
}

/**
 * @brief Connect using saved credentials
 */
bool connectWiFi() {
  #ifdef ESP32
    preferences.begin("thingslinker", true);
    if (!preferences.getBool("saved", false)) {
      preferences.end();
      Serial.println("[WiFi] No saved credentials");
      return false;
    }

    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("password", "");
    preferences.end();

    if (ssid.length() == 0) {
      Serial.println("[WiFi] No saved credentials");
      return false;
    }

    return connectWiFi(ssid.c_str(), password.c_str(), false);
  #else
    Serial.println("[WiFi] No saved credentials");
    return false;
  #endif
}

/**
 * @brief Disconnect from WiFi
 */
void disconnectWiFi() {
  WiFi.disconnect(true);
  Serial.println("[WiFi] Disconnected");
}

/**
 * @brief Check if WiFi is connected
 */
bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

/**
 * @brief Clear saved credentials
 */
void clearWiFiCredentials() {
  #ifdef ESP32
    preferences.begin("thingslinker", false);
    preferences.clear();
    preferences.end();
    Serial.println("[WiFi] ✓ Credentials cleared");
  #endif
}

/**
 * @brief Check if credentials are saved
 */
bool hasWiFiCredentials() {
  #ifdef ESP32
    preferences.begin("thingslinker", true);
    bool saved = preferences.getBool("saved", false);
    preferences.end();
    return saved;
  #else
    return false;
  #endif
}

/**
 * @brief Get WiFi IP address
 */
String getWiFiIP() {
  if (WiFi.status() == WL_CONNECTED) {
    return WiFi.localIP().toString();
  }
  return "0.0.0.0";
}

/**
 * @brief Get WiFi signal strength
 */
int getWiFiRSSI() {
  if (WiFi.status() == WL_CONNECTED) {
    return WiFi.RSSI();
  }
  return -100;
}
