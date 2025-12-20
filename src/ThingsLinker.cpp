/**
 * @file ThingsLinker.cpp
 * @brief Super simple implementation
 */

#include "ThingsLinker.h"

// Static variable for restart flag
bool ThingsLinker::_shouldRestart = false;

/**
 * @brief Constructor
 */
ThingsLinker::ThingsLinker(const char* authToken, const char* blueprintId)
  : _authToken(authToken),
    _blueprintId(blueprintId),
    _clientKey(nullptr),
    _secretKey(nullptr),
    _debugEnabled(true),
    _initialized(false),
    _lastCheck(0) {
}

/**
 * @brief Initialize ThingsLinker
 */
void ThingsLinker::begin(const char* clientKey, const char* secretKey) {
  _clientKey = clientKey;
  _secretKey = secretKey;

  Serial.begin(115200);
  delay(1000);

  Serial.println("\n========================================");
  Serial.println("   ThingsLinker IoT - Super Simple!");
  Serial.println("========================================");
  Serial.println("Chip ID: " + getChipID());
  Serial.println("========================================\n");

  // Try to connect to saved WiFi
  if (hasWiFiCredentials()) {
    Serial.println("[Setup] Found saved WiFi, connecting...");
    if (connectWiFi()) {
      Serial.println("[Setup] ✓ WiFi connected!");

      // Connect MQTT
      if (connectMQTT(_authToken, _blueprintId, _clientKey, _secretKey)) {
        Serial.println("[Setup] ✓ MQTT connected!");
        Serial.println("[Setup] ✓ Device ready!");
      }
    } else {
      // WiFi failed, start BLE
      Serial.println("[Setup] WiFi failed, starting BLE...");
      handleBLEProvisioning();
    }
  } else {
    // No WiFi credentials, start BLE
    Serial.println("[Setup] No WiFi found, starting BLE...");
    handleBLEProvisioning();
  }

  _initialized = true;
}

/**
 * @brief Main loop
 */
void ThingsLinker::run() {
  if (!_initialized) return;

  // Check if restart is requested (MUST be checked even during BLE provisioning)
  if (_shouldRestart) {
    Serial.println("[System] Restarting NOW!");
    Serial.flush();
    delay(100);
    ESP.restart();
  }

  // Don't run MQTT or connection checks while BLE is active
  if (isBLEActive()) {
    return;  // Let BLE handle everything during provisioning
  }

  // Process MQTT
  loopMQTT();

  // Check connections every 5 seconds
  unsigned long now = millis();
  if (now - _lastCheck > 5000) {
    _lastCheck = now;
    checkConnections();
  }
}

/**
 * @brief Handle BLE provisioning
 */
void ThingsLinker::handleBLEProvisioning() {
  String deviceName = String(BLE_DEVICE_NAME_PREFIX) + getChipID();

  // Callback not needed - restart happens directly in BLE callback
  // onBLEOnboardingComplete([]() {
  //   // Restart handled in TL_BLE.cpp
  // });

  // Set callback for when app disconnects (fallback if no confirmation received)
  onBLEDisconnected([]() {
    Serial.println("[BLE] App disconnected");
  });

  // Set callback for WiFi credentials
  onBLECredentialsReceived([](String ssid, String password) {
    Serial.println("[BLE] Connecting to WiFi...");

    bool connected = connectWiFi(ssid.c_str(), password.c_str(), true);

    if (connected) {
      Serial.println("[BLE] ✓ WiFi connected!");
      Serial.println("[BLE] Sending status to app...");
      sendBLEStatus(connected, getWiFiIP());
      Serial.println("[BLE] ✓ Status sent, waiting for app to disconnect...");
    } else {
      Serial.println("[BLE] ✗ WiFi connection failed!");
      Serial.println("[BLE] Sending failure status...");
      sendBLEStatus(false, "");
      Serial.println("[BLE] Keeping BLE active for retry...");
    }
  });

  // Start BLE
  startBLE(deviceName.c_str());

  Serial.println("\n========================================");
  Serial.println("  BLE PROVISIONING ACTIVE");
  Serial.println("========================================");
  Serial.println("1. Open ThingsLinker app");
  Serial.println("2. Scan for: " + deviceName);
  Serial.println("3. Enter WiFi credentials");
  Serial.println("========================================\n");
}

/**
 * @brief Check and reconnect if needed
 */
void ThingsLinker::checkConnections() {
  // Check WiFi - only attempt reconnection if credentials are saved
  if (!isWiFiConnected() && hasWiFiCredentials()) {
    Serial.println("[Check] WiFi disconnected, reconnecting...");
    connectWiFi();
  }

  // Check MQTT - only attempt reconnection if WiFi is connected
  if (isWiFiConnected() && !isMQTTConnected()) {
    Serial.println("[Check] MQTT disconnected, reconnecting...");
    connectMQTT(_authToken, _blueprintId, _clientKey, _secretKey);
  }
}

// ========== Widget Functions ==========

void ThingsLinker::button(const char* pin, bool value) {
  publishMQTT("Button", pin, value ? 1.0f : 0.0f);
}

void ThingsLinker::led(const char* pin, bool value) {
  publishMQTT("LED", pin, value ? 1.0f : 0.0f);
}

void ThingsLinker::gauge(const char* pin, float value) {
  publishMQTT("Gauge", pin, value);
}

void ThingsLinker::slider(const char* pin, float value) {
  publishMQTT("Slider", pin, value);
}

void ThingsLinker::send(const char* pin, float value) {
  publishMQTT("Value Display", pin, value);
}

void ThingsLinker::onButton(const char* pin, void (*callback)(bool value)) {
  subscribeMQTTButton("Button", pin, callback);
}

void ThingsLinker::onSlider(const char* pin, void (*callback)(float value)) {
  subscribeMQTT("Slider", pin, callback);
}

void ThingsLinker::onValue(const char* pin, void (*callback)(float value)) {
  subscribeMQTT("Value Display", pin, callback);
}

String ThingsLinker::buildTopic(const char* widgetType, const char* pin) {
  return "device/" + String(widgetType) + "/" + String(_blueprintId) + "/" +
         String(_authToken) + "/" + String(pin) + "/";
}

// ========== Status Functions ==========

bool ThingsLinker::wifiConnected() {
  return isWiFiConnected();
}

bool ThingsLinker::mqttConnected() {
  return isMQTTConnected();
}

bool ThingsLinker::bleActive() {
  return isBLEActive();
}

String ThingsLinker::getIP() {
  return getWiFiIP();
}

String ThingsLinker::getChipID() {
  return ::getChipID();
}

// ========== Storage Functions ==========

bool ThingsLinker::saveString(const char* key, const String& value) {
  return ::saveString(key, value);
}

String ThingsLinker::getString(const char* key, const String& defaultValue) {
  return ::getString(key, defaultValue);
}

bool ThingsLinker::saveInt(const char* key, int value) {
  return ::saveInt(key, value);
}

int ThingsLinker::getInt(const char* key, int defaultValue) {
  return ::getInt(key, defaultValue);
}

bool ThingsLinker::saveFloat(const char* key, float value) {
  return ::saveFloat(key, value);
}

float ThingsLinker::getFloat(const char* key, float defaultValue) {
  return ::getFloat(key, defaultValue);
}

bool ThingsLinker::saveBool(const char* key, bool value) {
  return ::saveBool(key, value);
}

bool ThingsLinker::getBool(const char* key, bool defaultValue) {
  return ::getBool(key, defaultValue);
}

bool ThingsLinker::hasKey(const char* key) {
  return ::hasKey(key);
}

bool ThingsLinker::removeKey(const char* key) {
  return ::removeKey(key);
}

void ThingsLinker::clearAllData() {
  ::clearAllData();
}

// ========== Advanced Functions ==========

void ThingsLinker::resetWiFi() {
  Serial.println("[Reset] Clearing WiFi credentials...");
  clearWiFiCredentials();
  disconnectWiFi();
  disconnectMQTT();

  Serial.println("[Reset] Starting BLE provisioning...");
  handleBLEProvisioning();
}

void ThingsLinker::debug(bool enable) {
  _debugEnabled = enable;
}

// Global instance pointer (for callbacks)
ThingsLinker* _globalThingsLinker = nullptr;
