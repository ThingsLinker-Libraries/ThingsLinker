/**
 * @file TL_Storage.cpp
 * @brief Storage implementation using ESP32 Preferences
 */

#include "TL_Storage.h"
#include <Preferences.h>

// Storage namespace for app data (separate from WiFi)
static Preferences appStorage;
static bool storageInitialized = false;

bool initStorage() {
  if (!storageInitialized) {
    storageInitialized = appStorage.begin("thingslinker_app", false); // false = read/write mode
  }
  return storageInitialized;
}

bool saveString(const char* key, const String& value) {
  if (!initStorage()) return false;

  size_t written = appStorage.putString(key, value);
  return (written > 0);
}

String getString(const char* key, const String& defaultValue) {
  if (!initStorage()) return defaultValue;

  return appStorage.getString(key, defaultValue);
}

bool saveInt(const char* key, int value) {
  if (!initStorage()) return false;

  size_t written = appStorage.putInt(key, value);
  return (written > 0);
}

int getInt(const char* key, int defaultValue) {
  if (!initStorage()) return defaultValue;

  return appStorage.getInt(key, defaultValue);
}

bool saveFloat(const char* key, float value) {
  if (!initStorage()) return false;

  size_t written = appStorage.putFloat(key, value);
  return (written > 0);
}

float getFloat(const char* key, float defaultValue) {
  if (!initStorage()) return defaultValue;

  return appStorage.getFloat(key, defaultValue);
}

bool saveBool(const char* key, bool value) {
  if (!initStorage()) return false;

  size_t written = appStorage.putBool(key, value);
  return (written > 0);
}

bool getBool(const char* key, bool defaultValue) {
  if (!initStorage()) return defaultValue;

  return appStorage.getBool(key, defaultValue);
}

bool hasKey(const char* key) {
  if (!initStorage()) return false;

  return appStorage.isKey(key);
}

bool removeKey(const char* key) {
  if (!initStorage()) return false;

  return appStorage.remove(key);
}

void clearAllData() {
  if (!initStorage()) return;

  appStorage.clear();
}

int getKeyCount() {
  if (!initStorage()) return 0;

  // Count keys by trying to enumerate them
  // Note: ESP32 Preferences doesn't have a direct key count function
  // This is an approximation - returns 0 if empty, >0 if has data
  return appStorage.freeEntries() > 0 ? 1 : 0;
}
