/**
 * @file 01_BLE_Provisioning.ino
 * @brief ThingsLinker BLE Provisioning Example
 *
 * This example demonstrates:
 * 1. BLE provisioning for WiFi credentials
 * 2. Automatic WiFi connection
 * 3. MQTT communication
 * 4. LED control via app
 *
 * Hardware:
 * - ESP32 board
 * - Built-in LED (GPIO 2) or external LED
 *
 * Usage:
 * 1. Upload this sketch to ESP32
 * 2. Open Serial Monitor (115200 baud)
 * 3. Open ThingsLinker mobile app
 * 4. Scan for BLE device "ThingsLinker_XXXXXX"
 * 5. Enter WiFi credentials in app
 * 6. Device connects to WiFi and MQTT
 * 7. Control LED from app using Button widget on V0
 */

#include <ThingsLinker.h>

// ========== Configuration ==========

// Device credentials (get these from ThingsLinker dashboard)
// IMPORTANT: All 4 credentials are mandatory
const char* AUTH_TOKEN = "EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s";
const char* BLUEPRINT_ID = "BLUEZ8hnUqddtfu5";  // MANDATORY for OTA and device control
const char* CLIENT_KEY = "client-6909e0dc170629c18aa1769e-72980cda832f45cc8e862a0b16e3d561";
const char* SECRET_KEY = "secret-6909e0dc170629c18aa1769e-504a9041f6554a49aaabaf83a897b26c1c31a497d25c4ef480bb6451580d2d2d";

// Hardware
#define LED_PIN 2
#define BUTTON_PIN 0  // Optional: physical button for re-provisioning

// ========== Global Objects ==========

ThingsLinker iot(AUTH_TOKEN, BLUEPRINT_ID);

bool ledState = false;
unsigned long lastButtonPress = 0;

// ========== Callbacks ==========

/**
 * Called when button is pressed from app
 */
void onButtonPressed(String pin, float value) {
  Serial.println("\n📱 Button press from app!");
  Serial.printf("  Pin: %s, Value: %.0f\n", pin.c_str(), value);

  // Update LED
  ledState = (value > 0);
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);

  // Send LED status back to app
  iot.setLED("V1", ledState);

  Serial.printf("💡 LED turned %s\n", ledState ? "ON" : "OFF");
}

/**
 * Called when WiFi connects
 */
void onWiFiConnected() {
  Serial.println("\n✓ WiFi connection established!");
  Serial.println("  IP: " + WiFi.localIP().toString());
}

/**
 * Called when MQTT connects
 */
void onMQTTConnected() {
  Serial.println("\n✓ MQTT connection established!");
  Serial.println("💡 Device is ready for control");
}

/**
 * Called when BLE provisioning completes
 */
void onProvisioningComplete() {
  Serial.println("\n✓ Provisioning complete!");
  Serial.println("🚀 Device is fully configured");
}

// ========== Setup ==========

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║   ThingsLinker BLE Provisioning Demo  ║");
  Serial.println("╚════════════════════════════════════════╝");

  // Setup hardware
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Test LED
  Serial.println("\n🔆 Testing LED...");
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  Serial.println("✓ LED test complete");

  // Initialize ThingsLinker
  Serial.println("\n🚀 Initializing ThingsLinker...");
  iot.enableDebug(true);

  // Set callbacks
  iot.onWiFiConnected(onWiFiConnected);
  iot.onMQTTConnected(onMQTTConnected);
  iot.onProvisioningComplete(onProvisioningComplete);

  // Begin (autoConnect = true will try to connect if credentials exist)
  iot.begin(AUTH_TOKEN, CLIENT_KEY, SECRET_KEY, true);

  // Subscribe to button control from app
  iot.subscribe("V0", WIDGET_BUTTON, onButtonPressed);

  // Check if already connected
  if (iot.isWiFiConnected() && iot.isMQTTConnected()) {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║  ✓ DEVICE READY                       ║");
    Serial.println("║  Control LED from app (Button on V0)  ║");
    Serial.println("╚════════════════════════════════════════╝\n");
  } else {
    // Start BLE provisioning
    Serial.println("\n📱 Starting BLE provisioning...");
    Serial.println("💡 Open ThingsLinker app to configure WiFi");

    // Start BLE with 5 minute timeout
    iot.startBLE(nullptr, 300);

    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║  BLE PROVISIONING ACTIVE               ║");
    Serial.println("║  1. Open ThingsLinker app              ║");
    Serial.println("║  2. Scan for BLE device                ║");
    Serial.println("║  3. Enter WiFi credentials             ║");
    Serial.println("╚════════════════════════════════════════╝\n");
  }
}

// ========== Loop ==========

void loop() {
  // Run ThingsLinker (handles WiFi, MQTT, BLE)
  iot.run();

  // Check for physical button press (long press = re-provision)
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (millis() - lastButtonPress > 50) { // Debounce
      unsigned long pressStart = millis();
      while (digitalRead(BUTTON_PIN) == LOW) {
        delay(10);
      }
      unsigned long pressDuration = millis() - pressStart;

      if (pressDuration > 3000) {
        // Long press - clear WiFi and restart BLE
        Serial.println("\n🔄 Re-provisioning requested...");
        iot.clearWiFiCredentials();
        iot.disconnectWiFi();
        iot.disconnectMQTT();

        delay(1000);

        Serial.println("📱 Starting BLE provisioning...");
        iot.startBLE(nullptr, 300);

        // Blink LED to indicate re-provisioning mode
        for (int i = 0; i < 5; i++) {
          digitalWrite(LED_PIN, HIGH);
          delay(200);
          digitalWrite(LED_PIN, LOW);
          delay(200);
        }
      } else {
        // Short press - toggle LED manually
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        Serial.printf("💡 LED toggled %s (manual)\n", ledState ? "ON" : "OFF");

        // Update app
        if (iot.isMQTTConnected()) {
          iot.setLED("V1", ledState);
        }
      }

      lastButtonPress = millis();
    }
  }

  delay(10);
}
