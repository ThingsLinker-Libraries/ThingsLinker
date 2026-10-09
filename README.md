```
  _______ _     _                 _ _       _
 |__   __| |   (_)               | (_)     | |
    | |  | |__  _ _ __   __ _ ___| |_ _ __ | | _____ _ __
    | |  | '_ \| | '_ \ / _` / __| | | '_ \| |/ / _ \ '__|
    | |  | | | | | | | | (_| \__ \ | | | | |   <  __/ |
    |_|  |_| |_|_|_| |_|\__, |___/_|_|_| |_|_|\_\___|_|
                         __/ |
                        |___/
```

# ThingsLinker — Arduino Library for ESP32

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](https://github.com/Thingslinker-Organization/ThingsLinker-Arduino-Library)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-ESP32-orange.svg)](https://www.espressif.com/en/products/socs/esp32)

Connect your ESP32 to the ThingsLinker IoT platform in just **3 lines of code**.

Two variants in one library:

| Variant | Class | Connectivity | Use when |
|---------|-------|-------------|----------|
| **WiFi** | `ThingsLinker` | WiFi (BLE provisioning) | Device is near a router |
| **GSM** | `ThingsLinkerGSM` | 4G LTE via Simcom A7672 | Device is mobile or remote |

Both use the **exact same widget API** — `gauge()`, `onSwitch()`, `run()` etc. work identically in both.

---

## Table of Contents

- [WiFi Variant — ThingsLinker](#wifi-variant--thingslinker)
  - [Quick Start](#quick-start-wifi)
  - [Widget Examples (WiFi)](#widget-examples-wifi)
- [GSM Variant — ThingsLinkerGSM](#gsm-variant--thingslinkergsmm)
  - [Quick Start (GSM)](#quick-start-gsm)
  - [Widget Examples (GSM)](#widget-examples-gsm)
  - [GPS Tracker](#gps-tracker)
- [OTA Firmware Updates](#ota-firmware-updates)
- [Storage API](#storage-api)
- [Quick Reference](#quick-reference)
- [Troubleshooting](#troubleshooting)

---

## WiFi Variant — ThingsLinker

### Quick Start (WiFi)

**Step 1 — Install dependencies** (Arduino Library Manager):
- **ArduinoJson** ≥ 6.x
- **PubSubClient** ≥ 2.8

**Step 2 — Get your credentials** from the Organisation Portal:
`Blueprints → [Blueprint] → Devices → [Device]`
- **Auth Token** — long hex string
- **Blueprint Key** — starts with `BLUE`
- **Client Key** and **Secret Key**

**Step 3 — Upload:**

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_KEY");

void setup() {
  Serial.begin(115200);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();
}
```

**First boot:** BLE starts automatically. Open the ThingsLinker app → find your device → enter WiFi credentials.
**Every boot after:** Connects to WiFi and MQTT automatically.

---

### Widget Examples (WiFi)

#### Gauge — Send a sensor value

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_KEY");

void setup() {
  Serial.begin(115200);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();

  static unsigned long last = 0;
  if (millis() - last >= 3000) {
    last = millis();

    float temperature = 25.0;          // replace with real sensor
    iot.gauge("V1", temperature);      // → Gauge widget on V1
  }
}
```

---

#### Chart — Live graph with history

```cpp
void loop() {
  iot.run();

  static unsigned long last = 0;
  if (millis() - last >= 5000) {
    last = millis();

    float humidity = 60.0;             // replace with real sensor
    iot.chart("V1", humidity);         // → Chart widget on V1
  }
}
```

---

#### Switch — App controls a relay

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_KEY");

#define RELAY_PIN 4

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  iot.onSwitch("V1", [](bool on) {          // Switch widget on V1
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
    Serial.println(on ? "Relay ON" : "Relay OFF");
  });
}

void loop() {
  iot.run();
}
```

---

#### Slider — App controls brightness

```cpp
iot.onSlider("V1", [](float value) {        // Slider widget on V1 (0–100)
  int brightness = (int)(value * 2.55f);    // 0–100 → 0–255
  analogWrite(LED_PIN, brightness);
});
```

---

#### Button — App triggers an action

```cpp
iot.onButton("V1", [](bool pressed) {       // Button widget on V1
  if (pressed) {
    // do something when button is tapped in app
    Serial.println("Button tapped!");
  }
});
```

---

#### RGB — App controls LED strip color

```cpp
iot.onRGB("V1", [](uint8_t r, uint8_t g, uint8_t b,
                    bool on, uint16_t count, const char* pattern) {
  // r, g, b   = color (0–255 each)
  // on         = strip on or off
  // count      = number of LEDs (set in app)
  // pattern    = effect name e.g. "Solid", "Blink"

  Serial.printf("Color: R%d G%d B%d  On:%s\n", r, g, b, on ? "yes" : "no");
  // drive your NeoPixel / FastLED strip here
});
```

---

#### Joystick — App controls direction

```cpp
iot.onJoystick("V1", [](float x, float y) {
  // x: left (−1.0) to right (+1.0)
  // y: back (−1.0) to forward (+1.0)

  if      (y >  0.5) Serial.println("FORWARD");
  else if (y < -0.5) Serial.println("BACKWARD");
  else if (x >  0.5) Serial.println("RIGHT");
  else if (x < -0.5) Serial.println("LEFT");
  else               Serial.println("STOP");
});
```

---

#### Map — Send GPS location

```cpp
float lat = 23.0225, lng = 72.5714;   // from your GPS module
iot.map("V1", lat, lng);              // → Map widget on V1
```

---

#### LED — Show alarm state in app

```cpp
bool alarm = (temperature > 35.0);
iot.led("V1", alarm);                 // → LED widget turns red/green
```

---

## GSM Variant — ThingsLinkerGSM

Use this when your device needs to work **without WiFi** — on the road, in the field, or anywhere with a 4G signal.

**Required hardware:**
- ESP32 (any variant)
- Simcom **A7672SA / A7672E / A7672S** (or SIM7672) 4G LTE modem

**Required library:** Only **ArduinoJson** — no TinyGSM, no PubSubClient needed.

### Wiring

```
ESP32 GPIO16 (RX2) ──── A7672 TX
ESP32 GPIO17 (TX2) ──── A7672 RX
ESP32 GPIO4        ──── A7672 PWRKEY
External 4V/500mA  ──── A7672 VCC
GND                ──── A7672 GND
```

---

### Quick Start (GSM)

```cpp
#include <ThingsLinkerGSM.h>

ThingsLinkerGSM iot(Serial2, "YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_KEY");

// ── Option A: PWRKEY wired to a GPIO ─────────────────────────────────────────
#define MODEM_PWR_PIN  4          // GPIO 4 → A7672 PWRKEY

// ── Option B: PWRKEY not connected (modem powers on automatically) ────────────
// #define MODEM_PWR_PIN  TL_NO_PWRKEY

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);    // RX=16, TX=17

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY",
            "airtelgprs.com",   // your SIM card APN
            MODEM_PWR_PIN);     // pass pin number OR TL_NO_PWRKEY
}

void loop() {
  iot.run();
}
```

That's it. The library handles modem power-on, network registration, SSL, MQTT connect, and auto-reconnect automatically.

**PWRKEY — which option do I use?**

| Situation | Define |
|-----------|--------|
| PWRKEY wired to an ESP32 GPIO (e.g. GPIO 4) | `#define MODEM_PWR_PIN 4` |
| PWRKEY not connected — modem auto-starts when power is applied | `#define MODEM_PWR_PIN TL_NO_PWRKEY` |
| PWRKEY tied directly to VCC on the breakout board | `#define MODEM_PWR_PIN TL_NO_PWRKEY` |

**Common APNs:**
| Carrier | APN |
|---------|-----|
| Airtel India | `airtelgprs.com` |
| Jio India | `jionet` |
| Vodafone India | `www` |
| AT&T US | `phone` |
| T-Mobile US | `fast.t-mobile.com` |

---

### Widget Examples (GSM)

Every widget function is **identical** to the WiFi variant — just use `ThingsLinkerGSM` instead of `ThingsLinker`.

#### Gauge — Send a sensor reading

```cpp
#include <ThingsLinkerGSM.h>

ThingsLinkerGSM iot(Serial2, "YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_KEY");

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY", "airtelgprs.com", 4);
}

void loop() {
  iot.run();

  static unsigned long last = 0;
  if (millis() - last >= 5000) {
    last = millis();

    float temperature = 28.5;          // replace with real sensor
    iot.gauge("V1", temperature);      // → Gauge widget on V1

    Serial.println("Sent: " + String(temperature) + " °C");
  }
}
```

---

#### Switch — App controls a relay over 4G

```cpp
#include <ThingsLinkerGSM.h>

ThingsLinkerGSM iot(Serial2, "YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_KEY");

#define RELAY_PIN 26

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  pinMode(RELAY_PIN, OUTPUT);

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY", "airtelgprs.com", 4);

  iot.onSwitch("V1", [](bool on) {              // Switch widget on V1
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
    Serial.println(on ? "Relay ON" : "Relay OFF");
  });
}

void loop() {
  iot.run();
}
```

---

#### Multiple widgets — Temperature + Humidity + Relay

```cpp
#include <ThingsLinkerGSM.h>

ThingsLinkerGSM iot(Serial2, "YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_KEY");

#define RELAY_PIN 26

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  pinMode(RELAY_PIN, OUTPUT);

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY", "airtelgprs.com", 4);

  iot.onSwitch("V3", [](bool on) {
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  });
}

void loop() {
  iot.run();

  static unsigned long last = 0;
  if (millis() - last >= 10000) {
    last = millis();

    float temp = 26.5;     // replace with DHT22 / DS18B20 etc.
    float hum  = 62.0;

    iot.gauge("V1", temp);   // Gauge widget — temperature
    iot.gauge("V2", hum);    // Gauge widget — humidity
    iot.chart("V4", temp);   // Chart widget — temperature history
  }
}
```

---

#### Check connection status

```cpp
if (iot.mqttConnected()) {
  Serial.println("Connected!");
  Serial.println("IP     : " + iot.getIP());
  Serial.println("Signal : " + String(iot.signalQuality()) + "/31");
}
```

---

### GPS Tracker

The **A7672 has a built-in GPS** — no external GPS module needed. The library handles all communication with the GNSS engine internally.

**Additional wiring:**
```
Active GPS antenna ──── A7672 ANT_GPS connector (uFL / SMA)
```

```cpp
#include <ThingsLinkerGSM.h>

ThingsLinkerGSM iot(Serial2, "YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_KEY");

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY", "airtelgprs.com", 4);

  iot.startGPS();   // start the built-in GNSS engine
}

void loop() {
  iot.run();   // GPS is polled automatically inside run()

  static unsigned long last = 0;
  if (millis() - last >= 5000) {
    last = millis();

    if (iot.gpsValid()) {
      iot.map    ("V1", iot.gpsLat(), iot.gpsLng());   // Map widget
      iot.gauge  ("V2", iot.gpsSpeed());                // Gauge — speed km/h
      iot.display("V3", iot.gpsAltitude());             // Display — altitude m
      iot.led    ("V4", true);                          // LED — fix locked

      Serial.printf("GPS: %.6f, %.6f  speed=%.1f km/h\n",
                    iot.gpsLat(), iot.gpsLng(), iot.gpsSpeed());
    } else {
      iot.led("V4", false);                             // LED — searching
      Serial.println("GPS: searching for fix...");
    }
  }
}
```

**GPS functions:**

| Function | Returns | Description |
|----------|---------|-------------|
| `iot.startGPS()` | — | Power on GNSS engine (call once in setup) |
| `iot.gpsValid()` | `bool` | `true` when fix acquired |
| `iot.gpsLat()` | `float` | Latitude in decimal degrees |
| `iot.gpsLng()` | `float` | Longitude in decimal degrees |
| `iot.gpsSpeed()` | `float` | Speed in km/h |
| `iot.gpsAltitude()` | `float` | Altitude in metres |

> **Note:** First GPS fix can take 30–90 seconds outdoors with a clear sky view. `gpsValid()` returns `false` until the fix is acquired.

---

## OTA Firmware Updates

Update your device firmware wirelessly — no USB cable needed.

```cpp
#include <ThingsLinker.h>   // or <ThingsLinkerGSM.h>

// ... iot.begin() in setup() ...

void loop() {
  iot.run();

  static unsigned long lastOTA = 0;
  if (millis() - lastOTA >= 300000UL) {    // check every 5 minutes
    lastOTA = millis();

    OTAResult r = iot.checkOTA();

    if (r == OTA_NO_UPDATE) Serial.println("Firmware up to date");
    if (r == OTA_FAILED)    Serial.println("Update failed — will retry");
    if (r == OTA_ERROR)     Serial.println("Could not reach server");
    // OTA_SUCCESS never reaches here — device restarts automatically
  }
}
```

**Portal setup:**
1. **OTA → New Shipping** — upload your compiled `.bin` file
2. **Set shipment → Live** — devices will detect it on next `checkOTA()`
3. Monitor progress in **OTA → Shipments**

> **Important:** Compile with **Tools → Partition Scheme → Default with OTA**. Use the same partition scheme for every firmware version on a device.

---

## Storage API

Persist values to ESP32 flash — survives power-off and reboot.

```cpp
// Save
iot.saveString("device_name", "Sensor-01");
iot.saveInt   ("threshold",   30);
iot.saveFloat ("last_lat",    23.0225);
iot.saveBool  ("alarm_on",    false);

// Load  (second argument = default if key not found)
String name  = iot.getString("device_name", "Unknown");
int    thr   = iot.getInt   ("threshold",   25);
float  lat   = iot.getFloat ("last_lat",    0.0);
bool   alarm = iot.getBool  ("alarm_on",    false);

// Manage
iot.hasKey("threshold");     // → true / false
iot.removeKey("threshold");  // delete one key
iot.clearAllData();          // delete all saved data
```

> Key names must be **≤ 15 characters** (ESP32 NVS limit).

---

## Quick Reference

### All publish functions (Device → App)

| Function | Widget | Description |
|----------|--------|-------------|
| `iot.gauge("V1", value)` | Gauge | Numeric value with dial |
| `iot.chart("V1", value)` | Chart | Value + stores history graph |
| `iot.display("V1", value)` | Value Display | Large number display |
| `iot.label("V1", value)` | Label | Text label with value |
| `iot.led("V1", true/false)` | LED | On/off indicator |
| `iot.button("V1", true/false)` | Button | Button press state |
| `iot.slider("V1", value)` | Slider | Slider position |
| `iot.map("V1", lat, lng)` | Map | GPS location pin |

### All subscribe functions (App → Device)

| Function | Widget | Callback receives |
|----------|--------|------------------|
| `iot.onSwitch("V1", cb)` | Switch | `bool on` |
| `iot.onButton("V1", cb)` | Button | `bool pressed` |
| `iot.onLED("V1", cb)` | LED | `bool on` |
| `iot.onSlider("V1", cb)` | Slider | `float value` |
| `iot.onValue("V1", cb)` | Value Display | `float value` |
| `iot.onRGB("V1", cb)` | RGB | `r, g, b, on, count, pattern` |
| `iot.onTimer("V1", cb)` | Timer | `float seconds` |
| `iot.onJoystick("V1", cb)` | Joystick | `float x, float y` |

### Status functions

| Function | Returns | Description |
|----------|---------|-------------|
| `iot.mqttConnected()` | `bool` | MQTT broker connected |
| `iot.wifiConnected()` | `bool` | WiFi connected *(WiFi only)* |
| `iot.networkConnected()` | `bool` | GPRS data active *(GSM only)* |
| `iot.getIP()` | `String` | Current IP address |
| `iot.signalQuality()` | `int` | GSM signal 0–31 *(GSM only)* |
| `iot.getChipID()` | `String` | Unique chip ID |

### Virtual Pins

Use `"V0"` through `"V124"` (125 pins total). Assign each pin to a widget in the Organisation Portal dashboard.

---

## Examples

Open via **File → Examples → ThingsLinker** in Arduino IDE:

### WiFi Examples
| Example | Description |
|---------|-------------|
| `00_Connection_Test` | Verify credentials and connectivity |
| `01_SimpleExample` | Minimum working sketch |
| `01_BLE_Provisioning` | BLE provisioning + re-provisioning via BOOT button |
| `02_BLE_Force_Provision` | Always starts in BLE mode |
| `02_LED_Control` | Control LED from app |
| `03_Temperature_Monitor` | Publish sensor to Gauge widget |
| `04_Advanced_Control` | Multiple widgets simultaneously |
| `05_Storage_Example` | Persist settings across reboots |
| `07_All_Widgets_Test` | All widget types in one sketch |
| `08_OTA_Update` | OTA v1.0 base firmware |
| `09_Clear_WiFi` | Erase WiFi credentials and re-provision |
| `10_Full_Feature_Test` | Full publish + subscribe with reconnect |
| `11_OTA_TestFirmware` | OTA v2.0 test firmware (used as the new `.bin`) |

### GSM Examples
| Example | Description |
|---------|-------------|
| `12_GSM_Connection` | Basic 4G connection + temperature publish + Switch |
| `13_GSM_Full_Example` | All widgets — Gauge, Chart, Switch, Slider, RGB, Joystick, OTA |
| `14_GSM_GPS_Tracker` | Live GPS tracking with built-in GNSS — Map, speed, altitude |

---

## Troubleshooting

### WiFi Issues
| Symptom | Fix |
|---------|-----|
| Device not visible in BLE scan | Restart ESP32; check no other BLE client is connected |
| WiFi fails every boot | Confirm SSID is 2.4 GHz; call `iot.resetWiFi()` to re-provision |
| MQTT auth failed (state -1) | Check client key and secret key from portal |
| MQTT connect failed (state -2) | Check internet access; confirm port 8883 is not blocked |

### GSM Issues
| Symptom | Fix |
|---------|-----|
| `Modem not responding` | Check RX/TX wiring, power supply (need 500 mA+), PWRKEY GPIO |
| `Network registration failed` | Check SIM card is inserted; check antenna; check carrier coverage |
| `GPRS activation failed` | Check APN string matches your SIM carrier |
| MQTT connect error 30 | Bad Client Key / Secret Key — verify from portal |
| MQTT connect error 32 | SSL handshake failed — check modem firmware version |
| MQTT connect error 3 | Socket connect failed — check SIM data plan is active |
| Signal quality 99 | No signal — check antenna connection |

### GPS Issues
| Symptom | Fix |
|---------|-----|
| `gpsValid()` always false | Move device outdoors with clear sky view; first fix can take 60–90 s |
| GPS never gets fix indoors | Normal — GNSS requires open sky. Use a window or external antenna |
| Speed shows 0 when moving slowly | Normal — GNSS speed is unreliable below ~5 km/h |

### OTA Issues
| Symptom | Fix |
|---------|-----|
| `OTA_ERROR` | Check server URL in `TL_Config.h`; confirm device has internet |
| `OTA_FAILED` | Verify `.bin` is valid; confirm shipment is set to Live |
| "No OTA partition found" | Re-flash via USB with **Tools → Partition Scheme → Default with OTA** |
| "Firmware too large" | Recompile new firmware with same partition scheme as base |

---

## Library Architecture

```
src/
├── TL_Config.h           Central config — broker, pins, timeouts, debug macros
├── TL_Storage.h/.cpp     NVS Preferences wrapper (shared by both variants)
├── TL_OTA.h/.cpp         OTA update via HTTP download + ESP32 flash
├── ThingsLinker.h/.cpp   WiFi variant — BLE provisioning, WiFi, TLS MQTT
├── ThingsLinkerGSM.h/.cpp GSM variant  — native A76XX AT commands, GPS
├── TL_BLE.h/.cpp         BLE provisioning module
├── TL_WiFi.h/.cpp        WiFi connect, NTP, reconnect
├── TL_MQTT.h/.cpp        PubSubClient wrapper (WiFi variant)
└── TL_Base64.h/.cpp      Base64 utility
```

---

## License

MIT — use it however you like.

## Support

- Email: info@thingslinker.com
- Website: https://thingslinker.com
