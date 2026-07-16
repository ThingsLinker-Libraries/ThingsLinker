/**
 * ========================================
 *   ThingsLinker - LED Control Example
 * ========================================
 *
 * Control an LED from the ThingsLinker app!
 * This example shows:
 * - How to control LED from app button
 * - How to send LED status to app
 */

#include <ThingsLinker.h>

// Your credentials (blueprintId is mandatory)
ThingsLinker iot("9a1977dd443b7d1f0ff4cbdaa4cbf05e1376b786dcf1da344d06a0d8df46845f", "BLUEZ8hnUqddtfu5");

// LED pin
const int LED_PIN = 2;  // Built-in LED on most ESP32 boards

void setup() {
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  delay(1000);
  
  // Initialize ThingsLinker with MQTT credentials
  iot.begin("client-6909e0dc170629c18aa1769e-72980cda832f45cc8e862a0b16e3d561", "secret-6909e0dc170629c18aa1769e");

  // Listen for button press from app on pin V0
  iot.onButton("V0", [](bool value) {
    // Turn LED on/off based on button
    digitalWrite(LED_PIN, value ? HIGH : LOW);
    Serial.println(value ? "LED ON" : "LED OFF");
  });
}

void loop() {
  // Run ThingsLinker
  iot.run();

  // // Send LED status to app every 2 seconds
  // static unsigned long lastSend = 0;
  // if (millis() - lastSend > 2000) {
  //   lastSend = millis();

  //   // Read LED state and send to app
  //   bool ledState = digitalRead(LED_PIN);
  //   iot.led("V1", ledState);  // Send to pin V1 (LED widget in app)
  // }
}

/*
 * ========================================
 * How to use:
 * ========================================
 *
 * 1. Upload this code to ESP32
 * 2. Open ThingsLinker app
 * 3. Connect device via BLE (first time only)
 * 4. Go to device dashboard
 * 5. Add a BUTTON widget on pin V0
 * 6. Add a LED widget on pin V1
 * 7. Press button in app - LED will turn on/off!
 * 8. LED widget will show real-time LED status
 *
 * ========================================
 */
