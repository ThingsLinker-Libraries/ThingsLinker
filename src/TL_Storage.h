/**
 * @file TL_Storage.h
 * @brief Storage management - Save and retrieve data from app
 *
 * This file handles data storage using ESP32 Preferences (flash):
 * - Save strings, numbers, booleans from mobile app
 * - Retrieve stored data
 * - Clear data
 * - Persistent storage (survives reboot)
 *
 * Super simple to use!
 *
 * Example:
 *   // Save data from app
 *   saveString("deviceName", "Living Room");
 *   saveInt("threshold", 25);
 *   saveBool("enabled", true);
 *
 *   // Read data
 *   String name = getString("deviceName", "Default");
 *   int threshold = getInt("threshold", 20);
 *   bool enabled = getBool("enabled", false);
 */

#ifndef TL_STORAGE_H
#define TL_STORAGE_H

#include <Arduino.h>

/**
 * @brief Initialize storage system
 * @return true if successful
 *
 * Called automatically by ThingsLinker.begin()
 */
bool initStorage();

/**
 * @brief Save string value
 * @param key Storage key (max 15 characters)
 * @param value String to save (max 4000 characters)
 * @return true if saved successfully
 *
 * Example:
 *   saveString("deviceName", "Kitchen Light");
 *   saveString("location", "Building A");
 */
bool saveString(const char* key, const String& value);

/**
 * @brief Get string value
 * @param key Storage key
 * @param defaultValue Return this if key not found
 * @return Stored string or defaultValue
 *
 * Example:
 *   String name = getString("deviceName", "Unnamed");
 */
String getString(const char* key, const String& defaultValue = "");

/**
 * @brief Save integer value
 * @param key Storage key (max 15 characters)
 * @param value Integer to save
 * @return true if saved successfully
 *
 * Example:
 *   saveInt("threshold", 25);
 *   saveInt("interval", 5000);
 */
bool saveInt(const char* key, int value);

/**
 * @brief Get integer value
 * @param key Storage key
 * @param defaultValue Return this if key not found
 * @return Stored integer or defaultValue
 *
 * Example:
 *   int threshold = getInt("threshold", 20);
 */
int getInt(const char* key, int defaultValue = 0);

/**
 * @brief Save float value
 * @param key Storage key (max 15 characters)
 * @param value Float to save
 * @return true if saved successfully
 *
 * Example:
 *   saveFloat("temperature", 25.5);
 *   saveFloat("latitude", 40.7128);
 */
bool saveFloat(const char* key, float value);

/**
 * @brief Get float value
 * @param key Storage key
 * @param defaultValue Return this if key not found
 * @return Stored float or defaultValue
 *
 * Example:
 *   float temp = getFloat("temperature", 0.0);
 */
float getFloat(const char* key, float defaultValue = 0.0);

/**
 * @brief Save boolean value
 * @param key Storage key (max 15 characters)
 * @param value Boolean to save
 * @return true if saved successfully
 *
 * Example:
 *   saveBool("ledEnabled", true);
 *   saveBool("alarmActive", false);
 */
bool saveBool(const char* key, bool value);

/**
 * @brief Get boolean value
 * @param key Storage key
 * @param defaultValue Return this if key not found
 * @return Stored boolean or defaultValue
 *
 * Example:
 *   bool enabled = getBool("ledEnabled", false);
 */
bool getBool(const char* key, bool defaultValue = false);

/**
 * @brief Check if key exists in storage
 * @param key Storage key to check
 * @return true if key exists
 *
 * Example:
 *   if (hasKey("deviceName")) {
 *     Serial.println("Name is set!");
 *   }
 */
bool hasKey(const char* key);

/**
 * @brief Remove a key from storage
 * @param key Storage key to remove
 * @return true if removed
 *
 * Example:
 *   removeKey("oldData");
 */
bool removeKey(const char* key);

/**
 * @brief Clear all stored data (except WiFi credentials)
 *
 * Example:
 *   clearAllData();
 */
void clearAllData();

/**
 * @brief Get total number of stored keys
 * @return Number of keys
 *
 * Example:
 *   int count = getKeyCount();
 *   Serial.println("Stored keys: " + String(count));
 */
int getKeyCount();

#endif // TL_STORAGE_H
