/**
 * @file TL_BLE.cpp
 * @brief Implementation of BLE provisioning
 */

#include "TL_BLE.h"
#include "TL_Config.h"
#include <ArduinoJson.h>

#ifdef ESP32

// Global variables
static BLEServer* bleServer = nullptr;
static BLECharacteristic* wifiCharacteristic = nullptr;
static BLECharacteristic* statusCharacteristic = nullptr;
static BLECharacteristic* confirmCharacteristic = nullptr;
static bool bleActive = false;
static void (*credentialsCallback)(String, String) = nullptr;
static void (*disconnectCallback)() = nullptr;
static void (*onboardingCompleteCallback)() = nullptr;

// BLE Server Callbacks
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("[BLE] Client connected");
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("[BLE] Client disconnected");

    // Call disconnect callback if set
    if (disconnectCallback) {
      disconnectCallback();
    }
  }
};

// BLE Characteristic Callbacks for WiFi credentials
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    String value = pCharacteristic->getValue().c_str();
    if (value.length() == 0) return;

    Serial.println("[BLE] Received data: " + value);

    // Parse JSON
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, value.c_str());

    if (error) {
      Serial.println("[BLE] JSON parse error!");
      return;
    }

    // Extract WiFi credentials
    const char* ssid = doc["ssid"];
    const char* password = doc["password"];

    if (ssid && password) {
      Serial.println("[BLE] WiFi credentials received!");
      Serial.println("  SSID: " + String(ssid));

      // Call callback
      if (credentialsCallback) {
        credentialsCallback(String(ssid), String(password));
      }
    }
  }
};

// BLE Characteristic Callbacks for onboarding confirmation
class ConfirmCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    String value = pCharacteristic->getValue().c_str();
    if (value.length() == 0) return;

    Serial.println("[BLE] Received confirmation: " + value);

    // Parse JSON
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, value.c_str());

    if (!error) {
      const char* status = doc["status"];
      if (status && String(status) == "complete") {
        Serial.println("[BLE] ✓ Onboarding confirmed by app!");
        Serial.println("[System] Restarting ESP32...");
        Serial.flush();

        // Direct restart - simplest approach
        delay(500);
        ESP.restart();
      }
    }
  }
};

/**
 * @brief Start BLE provisioning
 */
bool startBLE(const char* deviceName) {
  if (bleActive) {
    Serial.println("[BLE] Already active");
    return false;
  }

  Serial.println("[BLE] Starting...");
  Serial.println("[BLE] Device name: " + String(deviceName));

  // Initialize BLE
  BLEDevice::init(deviceName);

  // Create BLE Server
  bleServer = BLEDevice::createServer();
  bleServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service
  BLEService* pService = bleServer->createService(BLE_SERVICE_UUID);

  // WiFi Characteristic (writable with and without response)
  wifiCharacteristic = pService->createCharacteristic(
    BLE_WIFI_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  wifiCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // Status Characteristic (readable + notify)
  statusCharacteristic = pService->createCharacteristic(
    BLE_STATUS_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  statusCharacteristic->addDescriptor(new BLE2902());

  // Confirm Characteristic (writable - for app to send acknowledgment)
  confirmCharacteristic = pService->createCharacteristic(
    BLE_CONFIRM_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  confirmCharacteristic->setCallbacks(new ConfirmCharacteristicCallbacks());

  // Start service
  pService->start();

  // Start advertising
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();

  bleActive = true;
  Serial.println("[BLE] ✓ Started successfully!");
  Serial.println("[BLE] 💡 Open ThingsLinker app to connect");

  return true;
}

/**
 * @brief Stop BLE provisioning
 */
void stopBLE() {
  if (!bleActive) return;

  Serial.println("[BLE] Stopping...");

  if (bleServer) {
    bleServer->getAdvertising()->stop();
    BLEDevice::deinit(true);
    bleServer = nullptr;
    wifiCharacteristic = nullptr;
    statusCharacteristic = nullptr;
  }

  bleActive = false;
  Serial.println("[BLE] ✓ Stopped");
}

/**
 * @brief Check if BLE is active
 */
bool isBLEActive() {
  return bleActive;
}

/**
 * @brief Set callback for WiFi credentials
 */
void onBLECredentialsReceived(void (*callback)(String ssid, String password)) {
  credentialsCallback = callback;
}

/**
 * @brief Send status back to app
 */
void sendBLEStatus(bool connected, const String& ip) {
  if (!bleActive || !statusCharacteristic) return;

  StaticJsonDocument<256> doc;
  doc["status"] = connected ? "connected" : "failed";
  doc["ip"] = ip;

  String response;
  serializeJson(doc, response);

  statusCharacteristic->setValue(response.c_str());
  statusCharacteristic->notify();

  Serial.println("[BLE] Status sent: " + response);
}

/**
 * @brief Set callback for BLE disconnect
 */
void onBLEDisconnected(void (*callback)()) {
  disconnectCallback = callback;
}

/**
 * @brief Set callback for onboarding complete confirmation
 */
void onBLEOnboardingComplete(void (*callback)()) {
  onboardingCompleteCallback = callback;
}

#else

// Dummy implementation for non-ESP32 platforms
bool startBLE(const char* deviceName) {
  Serial.println("[BLE] Not supported on this platform");
  return false;
}

void stopBLE() {}
bool isBLEActive() { return false; }
void onBLECredentialsReceived(void (*callback)(String, String)) {}
void sendBLEStatus(bool connected, const String& ip) {}
void onBLEDisconnected(void (*callback)()) {}

#endif
