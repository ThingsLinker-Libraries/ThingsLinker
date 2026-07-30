/**
 * @file TL_Storage.cpp
 * @brief Persistent key-value storage using ESP32 Preferences (NVS flash).
 *
 * Stored in the "tl_app" namespace, separate from the WiFi
 * credentials namespace ("thingslinker"), so clearing app data does not
 * affect WiFi provisioning.
 */

#include "TL_Storage.h"
#include "TL_Config.h"
#include <Preferences.h>

static Preferences _store;
static bool        _open = false;

// Open the namespace lazily and keep it open for the lifetime of the firmware.
// Returns false if Preferences cannot initialise (should never happen on ESP32).
static bool ensureOpen() {
  if (!_open) {
    _open = _store.begin("tl_app", false);
    if (!_open) TL_LOG("[Storage] ✗ Failed to open NVS namespace");
  }
  return _open;
}

bool initStorage() {
  return ensureOpen();
}

bool saveString(const char* key, const String& value) {
  if (!ensureOpen()) return false;
  return _store.putString(key, value) > 0;
}

String getString(const char* key, const String& defaultValue) {
  if (!ensureOpen()) return defaultValue;
  return _store.getString(key, defaultValue);
}

bool saveInt(const char* key, int value) {
  if (!ensureOpen()) return false;
  return _store.putInt(key, value) > 0;
}

int getInt(const char* key, int defaultValue) {
  if (!ensureOpen()) return defaultValue;
  return _store.getInt(key, defaultValue);
}

bool saveFloat(const char* key, float value) {
  if (!ensureOpen()) return false;
  return _store.putFloat(key, value) > 0;
}

float getFloat(const char* key, float defaultValue) {
  if (!ensureOpen()) return defaultValue;
  return _store.getFloat(key, defaultValue);
}

bool saveBool(const char* key, bool value) {
  if (!ensureOpen()) return false;
  return _store.putBool(key, value) > 0;
}

bool getBool(const char* key, bool defaultValue) {
  if (!ensureOpen()) return defaultValue;
  return _store.getBool(key, defaultValue);
}

bool hasKey(const char* key) {
  if (!ensureOpen()) return false;
  return _store.isKey(key);
}

bool removeKey(const char* key) {
  if (!ensureOpen()) return false;
  return _store.remove(key);
}

void clearAllData() {
  if (!ensureOpen()) return;
  _store.clear();
  TL_LOG("[Storage] All app data cleared");
}

int getKeyCount() {
  if (!ensureOpen()) return 0;
  // ESP32 NVS Preferences exposes freeEntries() but not a direct key count.
  // The total NVS partition capacity is typically 97 entries per namespace.
  // Returning (capacity - free) gives an approximate key count.
  size_t free = _store.freeEntries();
  // NVS namespace capacity is platform-dependent but 97 is the typical default.
  const int NVS_NAMESPACE_CAPACITY = 97;
  int used = NVS_NAMESPACE_CAPACITY - (int)free;
  return (used > 0) ? used : 0;
}
