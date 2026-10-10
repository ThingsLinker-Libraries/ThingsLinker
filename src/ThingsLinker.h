/**
 * @file ThingsLinker.h
 * @brief ThingsLinker IoT Library for ESP32
 *
 * Connect your ESP32 to the ThingsLinker platform in just 3 steps:
 *
 *   // 1. Create
 *   ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");
 *
 *   void setup() {
 *     Serial.begin(115200);
 *     // 2. Start
 *     iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
 *   }
 *
 *   void loop() {
 *     // 3. Run
 *     iot.run();
 *   }
 *
 * The library handles everything automatically:
 *   - BLE provisioning (first boot — no WiFi credentials yet)
 *   - WiFi connection and reconnection
 *   - MQTT connection and reconnection
 *   - NTP time sync for accurate telemetry timestamps
 *   - Last Will and Testament for online/offline status
 *
 * Virtual pins V0–V124 (125 pins) match the ThingsLinker Arduino companion
 * library and the organisation portal widget configuration.
 *
 * Get credentials from the ThingsLinker organisation portal:
 *   Blueprints → [Blueprint] → Devices → [Device] → Auth Token / Client Key / Secret Key
 */

#ifndef THINGSLINKER_H
#define THINGSLINKER_H

#include <Arduino.h>
#include "TL_Config.h"
#include "TL_Value.h"
#include "TL_BLE.h"
#include "TL_WiFi.h"
#include "TL_MQTT.h"
#include "TL_Storage.h"
#include "TL_OTA.h"

class ThingsLinker {
public:

  // ── Construction ────────────────────────────────────────────────────────────

  /**
   * @param authToken   Device authentication token (from organisation portal)
   * @param blueprintId Blueprint ID (e.g. "BLUExxxxxxxxxxxxxxxx" — starts with "BLUE")
   */
  ThingsLinker(const char* authToken, const char* blueprintId);

  // ── Lifecycle ───────────────────────────────────────────────────────────────

  /**
   * Initialise the library. Call once in setup().
   *
   * Note: call Serial.begin(115200) in your sketch before begin() if you want
   * to see debug output from the very first lines.
   *
   * @param clientKey MQTT client key (from portal)
   * @param secretKey MQTT secret key (from portal)
   */
  void begin(const char* clientKey, const char* secretKey);

  /**
   * Process all library tasks. Call every iteration of loop().
   * Handles MQTT messages, auto-reconnect, and BLE provisioning.
   */
  void run();

  // ── Publish (device → app) ──────────────────────────────────────────────────
  //
  // Data types per widget (same rules as the portal "Data Type" setting):
  //
  //   Button, Switch, LED        Boolean
  //   Slider, Gauge, Chart       Integer or Float
  //   Value Display, Label       Integer, Float, Boolean or String
  //   Terminal                   String
  //   RGB, Timer, Map, Joystick  structured (dedicated methods)
  //
  // Methods taking a TLValue accept any of these directly — the data type is
  // taken from the argument type, e.g. 42 → Integer, 23.5f → Float,
  // true → Boolean, "Running" / String → String.

  /** Publish a Button state (1 = pressed, 0 = released).   Pin: V0–V124 */
  void button(const char* pin, bool value);

  /** Publish an LED state (1 = on, 0 = off).               Pin: V0–V124 */
  void led(const char* pin, bool value);

  /**
   * Publish GPS coordinates to a Map widget.
   * Sends {"v": lat, "lat": lat, "lng": lng, "t": timestamp}.
   *
   * Example:
   *   iot.map("V12", 23.0225f, 72.5714f);  // Ahmedabad, India
   */
  void map(const char* pin, float lat, float lng);

  /** Publish a value to a Gauge widget.                     Pin: V0–V124 */
  void gauge(const char* pin, float value);

  /** Publish a value to a Chart widget (stores history).    Pin: V0–V124 */
  void chart(const char* pin, float value);

  /**
   * Publish a value to a Value Display widget.              Pin: V0–V124
   * Accepts Integer, Float, Boolean or String.
   *
   * Example:
   *   iot.display("V3", 42);          // Integer
   *   iot.display("V3", 23.5f);       // Float
   *   iot.display("V3", "Running");   // String
   */
  void display(const char* pin, const TLValue& value);

  /**
   * Publish a value to a Label widget.                      Pin: V0–V124
   * Accepts Integer, Float, Boolean or String.
   *
   * Example:
   *   iot.label("V4", "Door open");
   *   iot.label("V4", String("Zone ") + zone);
   */
  void label(const char* pin, const TLValue& value);

  /**
   * Print a line to a Terminal widget.                      Pin: V0–V124
   * Numbers and booleans are accepted too and shown as text.
   *
   * The line is sent on the pin's display channel, not the Terminal control
   * channel, so it is never echoed back into your own onTerminal() callback.
   *
   * Example:
   *   iot.terminal("V5", "Boot complete");
   */
  void terminal(const char* pin, const TLValue& text);

  /** Publish a Slider position.                             Pin: V0–V124 */
  void slider(const char* pin, float value);

  /**
   * Generic publish — maps to Value Display widget.         Pin: V0–V124
   * Accepts Integer, Float, Boolean or String.
   */
  void send(const char* pin, const TLValue& value);

  // ── Subscribe (app → device) ────────────────────────────────────────────────

  /**
   * Register a callback for Button presses from the app.
   *
   * Example:
   *   iot.onButton("V0", [](bool pressed) {
   *     digitalWrite(LED_PIN, pressed ? HIGH : LOW);
   *   });
   */
  void onButton(const char* pin, void (*callback)(bool));

  /**
   * Register a callback for LED widget taps from the app (on/off).
   *
   * Example:
   *   iot.onLED("V5", [](bool on) {
   *     digitalWrite(LED_PIN, on ? HIGH : LOW);
   *   });
   */
  void onLED(const char* pin, void (*callback)(bool));

  /**
   * Register a callback for Switch toggles from the app.
   *
   * Example:
   *   iot.onSwitch("V1", [](bool on) {
   *     digitalWrite(RELAY_PIN, on ? HIGH : LOW);
   *   });
   */
  void onSwitch(const char* pin, void (*callback)(bool));

  /**
   * Register a callback for Slider values from the app.
   *
   * Example:
   *   iot.onSlider("V2", [](float pct) {
   *     ledcWrite(PWM_CHANNEL, (int)(pct * 2.55f));
   *   });
   */
  void onSlider(const char* pin, void (*callback)(float));

  /**
   * Register a callback for any numeric value from the app.
   *
   * Example:
   *   iot.onValue("V3", [](float v) { Serial.println(v); });
   */
  void onValue(const char* pin, void (*callback)(float));

  /**
   * Register a callback for text typed into a Terminal widget in the app.
   * The pointer is only valid during the callback — copy it to keep it.
   *
   * The apps publish with the MQTT retain flag, so the last text sent is
   * delivered again every time the device reconnects. Do not trigger one-off
   * actions such as a restart directly from it.
   *
   * Example:
   *   iot.onTerminal("V5", [](const char* text) {
   *     Serial.printf("Terminal: %s\n", text);
   *   });
   */
  void onTerminal(const char* pin, void (*callback)(const char* text));

  /**
   * Register a callback for RGB widget color changes from the app.
   * Receives individual r, g, b channels (0–255), on/off state,
   * LED count (number of pixels set in the app, 1–300), and
   * pattern name (e.g. "Solid", "Blink" — "" if not sent).
   *
   * Full payload: {"v":1,"r":0,"g":255,"b":204,"status":"ON","count":50,"pattern":"Solid","t":...}
   *
   * Example:
   *   iot.onRGB("V6", [](uint8_t r, uint8_t g, uint8_t b, bool on, uint16_t count, const char* pattern) {
   *     strip.clear();
   *     if (on) strip.fill(strip.Color(r, g, b), 0, count);
   *     strip.show();
   *     Serial.printf("Pattern: %s\n", pattern);
   *   });
   */
  void onRGB(const char* pin, void (*callback)(uint8_t r, uint8_t g, uint8_t b, bool on, uint16_t count, const char* pattern));

  /**
   * Register a callback for Timer widget values from the app.
   *
   * Example:
   *   iot.onTimer("V10", [](float seconds) {
   *     Serial.println("Timer: " + String((int)seconds) + "s");
   *   });
   */
  void onTimer(const char* pin, void (*callback)(float));

  /**
   * Register a callback for Joystick widget X/Y values from the app.
   * x and y are floats (typically -1.0 to 1.0).
   *
   * Example:
   *   iot.onJoystick("V13", [](float x, float y) {
   *     Serial.printf("X:%.2f  Y:%.2f\n", x, y);
   *   });
   */
  void onJoystick(const char* pin, void (*callback)(float x, float y));

  // ── Status ──────────────────────────────────────────────────────────────────

  bool   wifiConnected();   ///< true if WiFi is up
  bool   mqttConnected();   ///< true if MQTT is connected
  bool   bleActive();       ///< true if BLE provisioning is running
  String getIP();           ///< Current WiFi IP address
  String getChipID();       ///< ESP32 MAC-derived chip identifier

  // ── Persistent storage ──────────────────────────────────────────────────────
  // Key strings max 15 characters (ESP32 NVS limit).

  bool   saveString(const char* key, const String& value);
  String getString (const char* key, const String& defaultValue = "");

  bool saveInt(const char* key, int value);
  int  getInt (const char* key, int defaultValue = 0);

  bool  saveFloat(const char* key, float value);
  float getFloat (const char* key, float defaultValue = 0.0f);

  bool saveBool(const char* key, bool value);
  bool getBool (const char* key, bool defaultValue = false);

  bool hasKey   (const char* key);   ///< true if key exists
  bool removeKey(const char* key);   ///< Delete a single key
  void clearAllData();               ///< Erase all app data (not WiFi credentials)

  // ── OTA firmware updates ────────────────────────────────────────────────────

  /**
   * Check for a pending OTA firmware update and apply it if available.
   *
   * WiFi must be connected. On success the device restarts automatically.
   * Call this periodically (e.g. every 60 s) inside loop() after iot.run().
   *
   * @param apiServer  Backend base URL, no trailing slash.
   *                   Defaults to TL_API_SERVER defined in TL_Config.h.
   *                   Override per-call: iot.checkOTA("http://192.168.1.10:8000")
   * @return OTAResult  (OTA_NO_UPDATE / OTA_SUCCESS / OTA_FAILED / OTA_ERROR)
   *
   * Example:
   *   if (iot.wifiConnected()) {
   *     OTAResult r = iot.checkOTA();
   *     if (r == OTA_FAILED) Serial.println("OTA failed — will retry.");
   *   }
   */
  OTAResult checkOTA(const char* apiServer = TL_API_SERVER);

  // ── Advanced ────────────────────────────────────────────────────────────────

  /**
   * Set a custom brand name for BLE advertising.
   * The device will appear as "<brandName>_<ChipID>" in BLE scans.
   * Call this BEFORE begin().
   *
   * Default: "ThingsLinker"
   *
   * Example:
   *   iot.setBLEName("AcmeSensors");
   *   // Device appears as: AcmeSensors_BC6575C55494
   *   iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
   */
  void setBLEName(const char* brandName);

  /**
   * Clear saved WiFi credentials and restart BLE provisioning.
   * Useful for a factory-reset button or changing networks.
   */
  void resetWiFi();

  /**
   * Enable or disable library debug output on Serial.
   * Debug is enabled by default. Disabling reduces Serial traffic.
   *
   * Example:
   *   iot.debug(false);  // Silent mode
   */
  void debug(bool enable);

private:
  const char*   _authToken;
  const char*   _blueprintId;
  const char*   _clientKey;
  const char*   _secretKey;
  const char*   _bleBrandName;   // Custom BLE prefix (default: "ThingsLinker")
  bool          _initialized;
  unsigned long _lastCheck;

  void handleBLEProvisioning();
  void checkConnections();
};

#endif // THINGSLINKER_H
