/**
 * @file TL_WiFi.h
 * @brief WiFi management - connect, disconnect, save credentials
 *
 * This file handles ALL WiFi operations:
 * - Connect to WiFi
 * - Save credentials to flash
 * - Auto-reconnect
 * - Check connection status
 *
 * Super simple to use!
 */

#ifndef TL_WIFI_H
#define TL_WIFI_H

#include <Arduino.h>

/**
 * @brief Connect to WiFi
 * @param ssid WiFi network name
 * @param password WiFi password
 * @param saveCredentials Save to flash for next boot (default: true)
 * @return true if connected
 *
 * Example:
 *   connectWiFi("MyWiFi", "password123");
 */
bool connectWiFi(const char* ssid, const char* password, bool saveCredentials = true);

/**
 * @brief Connect using saved credentials
 * @return true if connected
 *
 * Example:
 *   if (connectWiFi()) {
 *     Serial.println("Connected!");
 *   }
 */
bool connectWiFi();

/**
 * @brief Disconnect from WiFi
 *
 * Example:
 *   disconnectWiFi();
 */
void disconnectWiFi();

/**
 * @brief Check if WiFi is connected
 * @return true if connected
 *
 * Example:
 *   if (isWiFiConnected()) {
 *     Serial.println("Online!");
 *   }
 */
bool isWiFiConnected();

/**
 * @brief Clear saved WiFi credentials
 *
 * Example:
 *   clearWiFiCredentials();
 */
void clearWiFiCredentials();

/**
 * @brief Check if WiFi credentials are saved
 * @return true if credentials exist
 *
 * Example:
 *   if (hasWiFiCredentials()) {
 *     connectWiFi(); // Use saved credentials
 *   }
 */
bool hasWiFiCredentials();

/**
 * @brief Get WiFi IP address
 * @return IP address as string
 *
 * Example:
 *   String ip = getWiFiIP();
 *   Serial.println("IP: " + ip);
 */
String getWiFiIP();

/**
 * @brief Get WiFi signal strength
 * @return RSSI in dBm (-100 to 0)
 *
 * Example:
 *   int rssi = getWiFiRSSI();
 *   Serial.println("Signal: " + String(rssi) + " dBm");
 */
int getWiFiRSSI();

#endif // TL_WIFI_H
