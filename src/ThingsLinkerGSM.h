/**
 * @file ThingsLinkerGSM.h
 * @brief ThingsLinker IoT Library — Simcom A7672 / SIM7672 4G GSM variant
 *
 * Drop-in alternative to ThingsLinker for devices that use a 4G GSM modem
 * instead of WiFi. Identical widget API — change only setup() credentials.
 *
 * Uses native A76XX MQTT AT commands (AT+CMQTT*) directly — no TinyGSM,
 * no PubSubClient dependency.
 *
 * ── Required Arduino libraries (install via Library Manager) ─────────────────
 *   ArduinoJson   — https://arduinojson.org
 *
 * ── Wiring (ESP32 + Simcom A7672 / SIM7672 module) ───────────────────────────
 *   ESP32 GPIO16 (RX2) ────── A7672 TX
 *   ESP32 GPIO17 (TX2) ────── A7672 RX
 *   ESP32 GPIO4        ────── A7672 PWRKEY  (active HIGH, ≥1 s pulse to power on)
 *   3.7–4.2 V / 500 mA ───── A7672 VCC     (use a dedicated LDO or LiPo cell)
 *   Common GND         ────── A7672 GND
 *
 * ── Quick start ──────────────────────────────────────────────────────────────
 *
 *   #include <ThingsLinkerGSM.h>
 *
 *   ThingsLinkerGSM iot(Serial2, "YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_KEY");
 *
 *   void setup() {
 *     Serial.begin(115200);
 *     Serial2.begin(115200, SERIAL_8N1, 16, 17);   // RX=16, TX=17
 *     iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY",
 *               "airtelgprs.com",  // APN for your SIM
 *               4);                // PWRKEY pin
 *   }
 *
 *   void loop() {
 *     iot.run();
 *     iot.gauge("V1", analogRead(A0) * 3.3f / 4095.0f);
 *     delay(2000);
 *   }
 */

#ifndef THINGSLINKER_GSM_H
#define THINGSLINKER_GSM_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "TL_Config.h"
#include "TL_Value.h"
#include "TL_Storage.h"
#include "TL_OTA.h"

// ─────────────────────────────────────────────────────────────────────────────
// Internal URC receive state machine
// ─────────────────────────────────────────────────────────────────────────────
enum _GsmRxState {
  GSM_RX_IDLE,
  GSM_RX_TOPIC,
  GSM_RX_PAYLOAD
};

// ─────────────────────────────────────────────────────────────────────────────
class ThingsLinkerGSM {
public:

  // ── Construction ────────────────────────────────────────────────────────────

  /**
   * @param serial      HardwareSerial connected to the GSM modem (use Serial2).
   *                    Call serial.begin(baud, SERIAL_8N1, rxPin, txPin) in
   *                    setup() BEFORE calling iot.begin().
   * @param authToken   Device auth token (from Organisation Portal → Devices)
   * @param blueprintId Blueprint key (e.g. "BLUExxxxxxxxxxxxxxxx")
   */
  ThingsLinkerGSM(HardwareSerial& serial,
                  const char*     authToken,
                  const char*     blueprintId);

  // ── Lifecycle ───────────────────────────────────────────────────────────────

  /**
   * Initialise the GSM modem, register on the cellular network, open a GPRS
   * data context, and connect to the ThingsLinker MQTT broker via native
   * A76XX AT+CMQTT* commands (TLS, no certificate verification).
   * Call once in setup().
   *
   * @param clientKey  MQTT client key (from Organisation Portal)
   * @param secretKey  MQTT secret key (from Organisation Portal)
   * @param apn        APN for your SIM card (e.g. "airtelgprs.com", "internet")
   * @param pwrPin     ESP32 GPIO wired to modem PWRKEY (-1 to skip power pulse)
   * @param gprsUser   GPRS username   — leave empty for most carriers
   * @param gprsPass   GPRS password   — leave empty for most carriers
   */
  void begin(const char* clientKey,
             const char* secretKey,
             const char* apn,
             int         pwrPin   = -1,
             const char* gprsUser = "",
             const char* gprsPass = "");

  /**
   * Process all library tasks. Call every iteration of loop().
   * Reads incoming URC lines from the modem, assembles MQTT messages,
   * dispatches callbacks, and auto-reconnects on connection loss.
   */
  void run();

  // ── Publish (device → app) ──────────────────────────────────────────────────

  /** Publish a Button state (1 = pressed, 0 = released).   Pin: V0–V124 */
  void button(const char* pin, bool value);

  /** Publish an LED state (1 = on, 0 = off).               Pin: V0–V124 */
  void led(const char* pin, bool value);

  /**
   * Publish GPS coordinates to a Map widget.
   * Sends {"v": lat, "lat": lat, "lng": lng, "t": timestamp}.
   */
  void map(const char* pin, float lat, float lng);

  /** Publish a value to a Gauge widget.                     Pin: V0–V124 */
  void gauge(const char* pin, float value);

  /** Publish a value to a Chart widget (stores history).    Pin: V0–V124 */
  void chart(const char* pin, float value);

  // display(), label(), send() and terminal() take a TLValue, so they accept
  // Integer, Float, Boolean or String directly (see TL_Value.h):
  //   iot.display("V3", 42);   iot.label("V4", "Door open");

  /** Publish a value to a Value Display widget.             Pin: V0–V124 */
  void display(const char* pin, const TLValue& value);

  /** Publish a value to a Label widget.                     Pin: V0–V124 */
  void label(const char* pin, const TLValue& value);

  /**
   * Print a line to a Terminal widget.                      Pin: V0–V124
   * Sent on the pin's display channel so it is not echoed back to onTerminal().
   */
  void terminal(const char* pin, const TLValue& text);

  /** Publish a Slider position.                             Pin: V0–V124 */
  void slider(const char* pin, float value);

  /** Generic publish — maps to Value Display widget.        Pin: V0–V124 */
  void send(const char* pin, const TLValue& value);

  // ── Subscribe (app → device) ────────────────────────────────────────────────

  /** Register a callback for Button presses from the app. */
  void onButton(const char* pin, void (*callback)(bool));

  /** Register a callback for LED widget taps from the app. */
  void onLED(const char* pin, void (*callback)(bool));

  /** Register a callback for Switch toggles from the app. */
  void onSwitch(const char* pin, void (*callback)(bool));

  /** Register a callback for Slider values from the app. */
  void onSlider(const char* pin, void (*callback)(float));

  /** Register a callback for any numeric value from the app. */
  void onValue(const char* pin, void (*callback)(float));

  /**
   * Register a callback for RGB widget color changes from the app.
   * Callback receives: r, g, b (0–255), on/off state, LED count (1–300),
   * and pattern name (e.g. "Solid", "Blink" — "" if not sent).
   */
  void onRGB(const char* pin,
             void (*callback)(uint8_t r, uint8_t g, uint8_t b,
                              bool on, uint16_t count, const char* pattern));

  /**
   * Register a callback for text typed into a Terminal widget in the app.
   * The pointer is only valid during the callback — copy it to keep it.
   */
  void onTerminal(const char* pin, void (*callback)(const char* text));

  /** Register a callback for Timer widget values from the app. */
  void onTimer(const char* pin, void (*callback)(float));

  /** Register a callback for Joystick X/Y values from the app. */
  void onJoystick(const char* pin, void (*callback)(float x, float y));

  // ── Status ──────────────────────────────────────────────────────────────────

  bool   networkConnected();  ///< true if GPRS data context is active
  bool   mqttConnected();     ///< true if MQTT broker is connected
  String getIP();             ///< Current GPRS IP address
  int    signalQuality();     ///< Signal quality 0–31 (99 = unknown)
  String getChipID();         ///< ESP32 MAC-derived chip identifier

  // ── Persistent storage ──────────────────────────────────────────────────────

  bool   saveString(const char* key, const String& value);
  String getString (const char* key, const String& defaultValue = "");
  bool   saveInt   (const char* key, int value);
  int    getInt    (const char* key, int defaultValue = 0);
  bool   saveFloat (const char* key, float value);
  float  getFloat  (const char* key, float defaultValue = 0.0f);
  bool   saveBool  (const char* key, bool value);
  bool   getBool   (const char* key, bool defaultValue = false);
  bool   hasKey    (const char* key);
  bool   removeKey (const char* key);
  void   clearAllData();

  // ── GPS (A7672 built-in GNSS engine) ────────────────────────────────────────

  /**
   * Start the A7672 integrated GPS/GLONASS engine.
   * Call once in setup() after begin(). The library polls the GNSS engine
   * automatically inside run() — no extra code needed in loop().
   */
  void startGPS();

  bool  gpsValid();     ///< true if a GPS fix has been acquired
  float gpsLat();       ///< Latitude  in decimal degrees (negative = South)
  float gpsLng();       ///< Longitude in decimal degrees (negative = West)
  float gpsSpeed();     ///< Ground speed in km/h
  float gpsAltitude();  ///< Altitude above sea level in metres

  // ── OTA firmware updates ────────────────────────────────────────────────────

  OTAResult checkOTA(const char* apiServer = TL_API_SERVER);

  // ── Debug ───────────────────────────────────────────────────────────────────

  void debug(bool enable);

  // ── Callback struct types (public so helpers in .cpp can access) ─────────────
  struct FloatCb {
    char pin[8];
    char widgetType[20];
    void (*fn)(float);
  };
  struct BoolCb {
    char pin[8];
    char widgetType[20];
    void (*fn)(bool);
  };
  struct TextCb {
    char pin[8];
    char widgetType[20];
    void (*fn)(const char*);
  };
  struct RGBCb {
    char pin[8];
    void (*fn)(uint8_t r, uint8_t g, uint8_t b,
               bool on, uint16_t count, const char* pattern);
  };
  struct JoyCb {
    char pin[8];
    void (*fn)(float x, float y);
  };

private:
  HardwareSerial& _serial;

  const char*   _authToken;
  const char*   _blueprintId;
  const char*   _clientKey;
  const char*   _secretKey;
  const char*   _apn;
  const char*   _gprsUser;
  const char*   _gprsPass;
  int           _pwrPin;
  bool          _initialized;
  bool          _mqttOk;
  bool          _networkOk;
  int           _sigQuality;
  String        _ipAddress;
  unsigned long _lastCheck;

  // ── Callback tables ─────────────────────────────────────────────────────────
  FloatCb _floatCbs[MAX_SUBSCRIPTIONS];
  BoolCb  _boolCbs[MAX_SUBSCRIPTIONS];
  RGBCb   _rgbCbs[MAX_SUBSCRIPTIONS];
  JoyCb   _joyCbs[MAX_SUBSCRIPTIONS];
  TextCb  _textCbs[MAX_SUBSCRIPTIONS];
  int     _floatCount;
  int     _boolCount;
  int     _rgbCount;
  int     _joyCount;
  int     _textCount;

  // ── GPS state ────────────────────────────────────────────────────────────────
  bool          _gpsStarted;
  bool          _gpsValid;
  float         _gpsLat;
  float         _gpsLng;
  float         _gpsSpeedKmh;
  float         _gpsAltM;
  unsigned long _lastGpsUpdate;

  // ── Incoming message assembly buffers (URC parser) ───────────────────────────
  _GsmRxState _rxState;
  char        _rxTopic[256];
  char        _rxPayload[512];
  int         _rxTopicExpected;
  int         _rxPayloadExpected;

  // ── AT command layer ─────────────────────────────────────────────────────────
  void    _sendAT(const char* cmd);
  bool    _waitOK(uint32_t timeoutMs = 3000);
  // Read one line; returns false on timeout.  Line written into buf (max len).
  bool    _readLine(char* buf, int len, uint32_t timeoutMs = 3000);
  // Read exactly n bytes into buf; returns false on timeout.
  bool    _readBytes(char* buf, int n, uint32_t timeoutMs = 5000);
  // Send AT and block until response contains "OK" or "ERROR"; returns true=OK.
  bool    _atCmd(const char* cmd, uint32_t timeoutMs = 3000);
  // Send AT, wait for prompt ">", write data, then wait for OK.
  bool    _atDataCmd(const char* cmd, const char* data, int dataLen,
                     uint32_t timeoutMs = 5000);

  // ── Setup helpers ────────────────────────────────────────────────────────────
  void    _powerKey();
  bool    _waitModem(uint32_t timeoutMs = 30000);
  bool    _connectNetwork();
  bool    _connectMQTT();
  void    _subscribeAll();
  void    _syncTime();
  void    _checkConnections();

  // ── MQTT AT helpers ──────────────────────────────────────────────────────────
  bool    _mqttPublishAT(const char* topic, const char* payload);
  bool    _mqttSubscribeAT(const char* topic);

  // ── GPS internals ────────────────────────────────────────────────────────────
  void    _updateGPS();
  bool    _parseGpsInfo(const char* line);

  // ── URC / message handling ───────────────────────────────────────────────────
  void    _processLine(const char* line);
  void    _dispatchMessage(const char* topic, const char* payload);

  // ── Misc helpers ─────────────────────────────────────────────────────────────
  String        _buildTopic(const char* widgetType, const char* pin);
  void          _publish(const char* widgetType, const char* pin, const TLValue& value);
  void          _publishMap(const char* pin, float lat, float lng);
  void          _subscribe(const char* widgetType, const char* pin);
  unsigned long _getTimestamp();
};

#endif // THINGSLINKER_GSM_H
