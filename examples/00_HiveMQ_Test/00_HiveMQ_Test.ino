/**
 * ========================================
 *   ThingsLinker - HiveMQ Test Example
 * ========================================
 *
 * This example is for TESTING ONLY using HiveMQ public broker.
 * Perfect for testing the library without backend setup.
 *
 * IMPORTANT: HiveMQ is a PUBLIC broker - anyone can see your data!
 * Use this ONLY for testing, not for production.
 *
 * Configuration:
 * - MQTT Broker: broker.hivemq.com (configured in TL_Config.h)
 * - No authentication required (leave credentials empty)
 * - Test MQTT connection without ThingsLinker backend
 */

#include <ThingsLinker.h>

// Step 1: Create ThingsLinker object
// For HiveMQ testing, use any test values:
ThingsLinker iot("TEST_AUTH_TOKEN", "TEST_BLUEPRINT_ID");

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n========================================");
  Serial.println("  ThingsLinker - HiveMQ Test Mode");
  Serial.println("========================================");
  Serial.println("Broker: broker.hivemq.com (public)");
  Serial.println("WARNING: Public broker - data is visible to anyone!");
  Serial.println("========================================\n");

  // Step 2: Initialize with EMPTY credentials for HiveMQ
  // HiveMQ public broker doesn't require authentication
  iot.begin("", "");

  Serial.println("Library initialized!");
  Serial.println("Connect to WiFi using ThingsLinker app via BLE");
}

void loop() {
  // Step 3: Just call run()
  iot.run();

  // Optional: Send test data every 5 seconds
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    lastSend = millis();

    // Send random sensor value
    float temperature = random(20, 30) + random(0, 100) / 100.0;
    iot.send("V0", temperature);

    Serial.println("Sent temperature: " + String(temperature) + "°C");
  }
}

/*
 * ========================================
 * Testing Steps:
 * ========================================
 *
 * 1. Make sure TL_Config.h is configured for HiveMQ:
 *    #define MQTT_SERVER "broker.hivemq.com"
 *    #define MQTT_PORT 1883
 *
 * 2. Upload this sketch to ESP32
 *
 * 3. Open Serial Monitor (115200 baud)
 *
 * 4. Connect to device via ThingsLinker app:
 *    - Scan for "ThingsLinker_XXXXXX"
 *    - Send WiFi credentials
 *    - Device will connect to WiFi and MQTT
 *
 * 5. You should see:
 *    [WiFi] ✓ Connected!
 *    [MQTT] Connecting to broker.hivemq.com
 *    [MQTT] Connecting without authentication (test mode)...
 *    [MQTT] ✓ Connected!
 *
 * 6. Test MQTT using online HiveMQ client:
 *    http://www.hivemq.com/demos/websocket-client/
 *    - Connect to broker.hivemq.com:8000
 *    - Subscribe to: device/Value Display/TEST_BLUEPRINT_ID/TEST_AUTH_TOKEN/V0/
 *    - You should see temperature data
 *
 * ========================================
 * Switch to Production:
 * ========================================
 *
 * When ready for production:
 * 1. Edit TL_Config.h:
 *    - Comment out HiveMQ
 *    - Uncomment mqtt.thingslinker.com
 * 2. Get real credentials from ThingsLinker portal
 * 3. Update: iot("REAL_AUTH_TOKEN", "REAL_BLUEPRINT_ID")
 * 4. Update: iot.begin("REAL_CLIENT_KEY", "REAL_SECRET_KEY")
 *
 * ========================================
 */
