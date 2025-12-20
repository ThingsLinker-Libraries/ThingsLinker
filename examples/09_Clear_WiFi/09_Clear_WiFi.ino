/**
 * ========================================
 *   ThingsLinker - Clear WiFi & Re-Provision
 * ========================================
 *
 * Use this sketch to clear saved WiFi credentials
 * and restart BLE provisioning.
 *
 * Perfect for:
 * - Testing BLE provisioning multiple times
 * - Changing WiFi networks
 * - Factory reset
 *
 * Usage:
 * 1. Upload this sketch
 * 2. Wait for "BLE PROVISIONING ACTIVE"
 * 3. Open ThingsLinker app
 * 4. Scan and provision device
 */

#include <ThingsLinker.h>

// Your device credentials
// Get these from ThingsLinker organization portal
ThingsLinker iot("YOUR_DEVICE_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n========================================");
  Serial.println("  ThingsLinker - Clear WiFi");
  Serial.println("========================================");

  // Clear saved WiFi and start BLE provisioning
  Serial.println("Clearing saved WiFi credentials...");
  iot.resetWiFi();

  // Initialize with your MQTT credentials
  // For HiveMQ testing, use empty strings: iot.begin("", "");
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  Serial.println("✓ WiFi cleared!");
  Serial.println("✓ BLE provisioning started!");
  Serial.println("\nOpen ThingsLinker app to provision device.");
  Serial.println("========================================\n");
}

void loop() {
  // Keep running to handle BLE provisioning
  iot.run();

  // Optional: Send test data after WiFi connects
  static unsigned long lastSend = 0;
  if (iot.wifiConnected() && millis() - lastSend > 5000) {
    lastSend = millis();

    float temperature = random(20, 30) + random(0, 100) / 100.0;
    iot.send("V0", temperature);

    Serial.println("Sent temperature: " + String(temperature) + "°C");
  }
}

/*
 * ========================================
 * Expected Serial Output:
 * ========================================
 *
 * ThingsLinker - Clear WiFi
 * Clearing saved WiFi credentials...
 * [Reset] Clearing WiFi credentials...
 * [Reset] Starting BLE provisioning...
 * [BLE] Starting...
 * [BLE] Device name: ThingsLinker_BC6575C55494
 * [BLE] ✓ Started successfully!
 *
 * ========================================
 *   BLE PROVISIONING ACTIVE
 * ========================================
 * 1. Open ThingsLinker app
 * 2. Scan for: ThingsLinker_BC6575C55494
 * 3. Enter WiFi credentials
 * ========================================
 *
 * [Wait for app to send credentials...]
 *
 * [BLE] Client connected
 * [BLE] Received data: {"ssid":"MyWiFi","password":"password"}
 * [BLE] WiFi credentials received!
 * [WiFi] Connecting to: MyWiFi
 * ......
 * [WiFi] ✓ Connected!
 * [WiFi] IP: 192.168.1.17
 * [BLE] Status sent: {"status":"connected","ip":"192.168.1.17"}
 * [BLE] ✓ WiFi connected!
 * [BLE] Stopping...
 * [MQTT] Connecting to broker.hivemq.com
 * [MQTT] ✓ Connected!
 *
 * Sent temperature: 23.45°C
 * Sent temperature: 24.67°C
 *
 * ========================================
 * After provisioning:
 * ========================================
 *
 * Device now has WiFi credentials saved!
 * On next reboot, it will auto-connect to WiFi
 * and skip BLE provisioning.
 *
 * To re-provision:
 * - Upload this sketch again, OR
 * - Call iot.resetWiFi() in your code
 *
 * ========================================
 */
