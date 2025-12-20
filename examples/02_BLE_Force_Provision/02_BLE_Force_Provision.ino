/**
 * Force BLE Provisioning Mode
 *
 * This sketch ALWAYS starts in BLE provisioning mode,
 * ignoring any saved WiFi credentials.
 *
 * Use this when:
 * - Testing BLE provisioning flow
 * - Device needs to be re-provisioned
 * - WiFi credentials have changed
 */

#include <ThingsLinker.h>

// Initialize ThingsLinker (credentials not needed for BLE)
ThingsLinker iot("TEST", "TEST");

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n========================================");
  Serial.println("   FORCE BLE PROVISIONING MODE");
  Serial.println("========================================\n");

  // Clear any saved WiFi credentials
  Serial.println("[Setup] Clearing saved WiFi credentials...");
  iot.resetWiFi();

  // Start BLE provisioning (empty credentials forces BLE mode)
  Serial.println("[Setup] Starting BLE provisioning...");
  iot.begin("", "");

  Serial.println("\n========================================");
  Serial.println("  READY FOR APP CONNECTION");
  Serial.println("========================================");
  Serial.println("1. Open ThingsLinker app");
  Serial.println("2. Go to Devices → Add New Device");
  Serial.println("3. Scan QR code or enter barcode");
  Serial.println("4. Enter WiFi credentials");
  Serial.println("5. Select this device from BLE list");
  Serial.println("========================================\n");
}

void loop() {
  iot.run();
}
