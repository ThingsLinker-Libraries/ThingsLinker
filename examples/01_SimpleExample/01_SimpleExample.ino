/**
 * ========================================
 *   ThingsLinker - Simplest Example
 * ========================================
 *
 * This example shows how easy it is to use ThingsLinker!
 * Just 3 steps:
 * 1. Create ThingsLinker object
 * 2. Call begin() in setup
 * 3. Call run() in loop
 *
 * That's it! The library handles everything automatically:
 * - BLE provisioning (if no WiFi saved)
 * - WiFi connection
 * - MQTT connection
 * - Auto-reconnection
 */

#include <ThingsLinker.h>

// Step 1: Create ThingsLinker object with your device credentials
// IMPORTANT: blueprintId is mandatory (required for device identification and OTA updates)
ThingsLinker iot("YOUR_DEVICE_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

void setup() {
  // Step 2: Initialize with your MQTT credentials
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  // That's it! Library handles everything automatically
}

void loop() {
  // Step 3: Just call run() - it handles all communication
  iot.run();
}

/*
 * ========================================
 * What happens automatically:
 * ========================================
 *
 * First Time (No WiFi Saved):
 * 1. BLE starts automatically (name: ThingsLinker_XXXXXX)
 * 2. Connect with ThingsLinker app
 * 3. Send WiFi credentials
 * 4. Device connects to WiFi
 * 5. BLE stops automatically
 * 6. MQTT connects
 * 7. Device ready!
 *
 * Next Time:
 * 1. Device remembers WiFi
 * 2. Auto-connects to WiFi
 * 3. Auto-connects to MQTT
 * 4. Device ready in seconds!
 *
 * ========================================
 * Where to get your credentials:
 * ========================================
 *
 * 1. Login to ThingsLinker organization portal
 * 2. Create a Blueprint (device template)
 * 3. Create a Device and assign it to the Blueprint
 * 4. Copy these values from the device page:
 *    - Device Auth Token (required)
 *    - Blueprint ID (required - e.g., "BLUEZ8hnUqddtfu5")
 *    - Client Key (required)
 *    - Secret Key (required)
 * 5. Paste them in the code above
 *
 * Example credentials format:
 *   authToken = "EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s"
 *   blueprintId = "BLUEZ8hnUqddtfu5"
 *   clientKey = "client-6909e0dc170629c18aa1769e-72980cda832f45cc8e862a0b16e3d561"
 *   secretKey = "secret-6909e0dc170629c18aa1769e-504a9041f6554a49aaabaf83a897b26c1c31a497d25c4ef480bb6451580d2d2d"
 *
 * ========================================
 */
