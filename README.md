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

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](https://github.com/thingslinker/arduino-library)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-ESP32-orange.svg)](https://www.espressif.com/en/products/socs/esp32)

Connect your ESP32 to the ThingsLinker IoT platform in just **3 lines of code**.

## Features

- **BLE Provisioning** — Configure WiFi from the mobile app, no hardcoded passwords
- **Auto WiFi & MQTT** — Connects on boot, auto-reconnects on drop
- **TLS MQTT** — Encrypted connection to `mqtt.thingslinker.com:8883`
- **NTP Timestamps** — Accurate Unix timestamps on every telemetry payload
- **125 Virtual Pins** — V0–V124, matching the organisation portal
- **Persistent Storage** — Save settings to ESP32 flash via `Preferences`
- **MQTT Last Will** — Automatic online/offline status via retained messages
- **OTA Updates** — Download and flash new firmware automatically over WiFi

## Quick Start

### 1. Install Dependencies

Via Arduino Library Manager:
- **ArduinoJson** ≥ 6.x
- **PubSubClient** ≥ 2.8

### 2. Get Your Credentials

Log in to the ThingsLinker organisation portal:
`Blueprints → [Blueprint] → Devices → [Device]`

Copy:
- **Device Auth Token** — `~64 hex characters`
- **Blueprint ID** — starts with `BLUE`, e.g. `BLUExxxxxxxxxx`
- **Client Key** — starts with `client-`
- **Secret Key** — starts with `secret-`

### 3. Upload Your Sketch

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

void setup() {
  Serial.begin(115200);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();
}
```

**First boot:** BLE starts automatically. Open the ThingsLinker app, find your device, and enter WiFi credentials.
**All subsequent boots:** Device connects to WiFi and MQTT automatically in seconds.

## Widget Examples

Each example below is complete — copy, paste, fill in your credentials, and upload.

---

### Gauge Widget

Send a numeric value to the Gauge widget in the app (temperature, humidity, voltage, etc.).

App widget: **Gauge** on pin `V0`

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

void setup() {
  Serial.begin(115200);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();

  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 3000) {
    lastSend = millis();

    float temperature = 25.0 + random(-50, 50) / 10.0;  // simulated sensor
    iot.gauge("V0", temperature);

    Serial.println("Temperature: " + String(temperature) + " °C");
  }
}
```

---

### Chart Widget

Send data points to the Chart widget. The app stores history and draws a live graph.

App widget: **Chart** on pin `V0`

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

void setup() {
  Serial.begin(115200);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();

  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 5000) {
    lastSend = millis();

    float humidity = 60.0 + random(-100, 100) / 10.0;  // simulated sensor
    iot.chart("V0", humidity);

    Serial.println("Humidity: " + String(humidity) + " %");
  }
}
```

---

### Value Display Widget

Send a value to the Value Display widget (shows a large number in the app).

App widget: **Value Display** on pin `V0`

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

void setup() {
  Serial.begin(115200);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();

  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 3000) {
    lastSend = millis();

    float pressure = 1013.0 + random(-30, 30) / 10.0;  // simulated sensor
    iot.display("V0", pressure);

    Serial.println("Pressure: " + String(pressure) + " hPa");
  }
}
```

---

### Label Widget

Send a value to the Label widget (displays text + number in the app).

App widget: **Label** on pin `V0`

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

void setup() {
  Serial.begin(115200);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();

  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 3000) {
    lastSend = millis();

    float voltage = 3.3 + random(-10, 10) / 100.0;  // simulated sensor
    iot.label("V0", voltage);

    Serial.println("Voltage: " + String(voltage) + " V");
  }
}
```

---

### LED Widget (Device → App)

Control the LED widget state in the app from the device (e.g. show a sensor alarm).

App widget: **LED** on pin `V0`

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();

  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 3000) {
    lastSend = millis();

    bool alarmOn = (random(0, 10) > 7);    // simulate alarm condition
    iot.led("V0", alarmOn);                 // update LED widget in app
    digitalWrite(LED_PIN, alarmOn ? HIGH : LOW);

    Serial.println("Alarm: " + String(alarmOn ? "ON" : "OFF"));
  }
}
```

---

### Map Widget

Send GPS coordinates to the Map widget. The app shows your device's location on a map.

App widget: **Map** on pin `V0`

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

void setup() {
  Serial.begin(115200);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();

  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 5000) {
    lastSend = millis();

    // Replace with real GPS values from your GPS module
    float lat = 23.0225f;
    float lng = 72.5714f;

    iot.map("V0", lat, lng);

    Serial.printf("Location: %.4f, %.4f\n", lat, lng);
  }
}
```

---

### Button Widget (App → Device)

App button controls the built-in LED. When you tap the button in the app, the LED toggles on the device.

App widget: **Button** on pin `V0`

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  iot.onButton("V0", [](bool pressed) {
    digitalWrite(LED_PIN, pressed ? HIGH : LOW);
    Serial.println("Button: LED " + String(pressed ? "ON" : "OFF"));
  });
}

void loop() {
  iot.run();
}
```

---

### LED Widget (App → Device)

Tap the LED widget in the app to toggle it on/off. The device receives the command and controls a physical output.

App widget: **LED** on pin `V0`

> Do **not** call `iot.led("V0", ...)` inside this callback — it causes an infinite echo loop.

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  iot.onLED("V0", [](bool on) {
    digitalWrite(LED_PIN, on ? HIGH : LOW);
    Serial.println("LED widget: " + String(on ? "ON" : "OFF"));
  });
}

void loop() {
  iot.run();
}
```

---

### Switch Widget

Toggle switch in the app controls a relay or any on/off output on the device.

App widget: **Switch** on pin `V0`

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

#define RELAY_PIN 4

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  iot.onSwitch("V0", [](bool on) {
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
    Serial.println("Switch: Relay " + String(on ? "ON" : "OFF"));
  });
}

void loop() {
  iot.run();
}
```

---

### Slider Widget

Drag the slider in the app to control brightness, speed, or any variable value on the device.

App widget: **Slider** on pin `V0` (set range 0–100 in portal)

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  iot.onSlider("V0", [](float value) {
    // value is 0–100 (range you set in portal)
    int brightness = (int)(value * 2.55f);  // convert to 0–255
    analogWrite(LED_PIN, brightness);

    Serial.println("Slider: " + String((int)value) + "% brightness");
  });
}

void loop() {
  iot.run();
}
```

---

### RGB Widget

Pick a color in the app to control an RGB LED strip or any RGB output on the device.

App widget: **RGB** on pin `V0`

```cpp
#include <ThingsLinker.h>
#include <Adafruit_NeoPixel.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

#define PIXEL_PIN   5
#define PIXEL_COUNT 8

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show();

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  // Payload: {"v":1,"r":0,"g":255,"b":204,"status":"ON","count":50,"pattern":"Solid","t":...}
  iot.onRGB("V0", [](uint8_t r, uint8_t g, uint8_t b, bool on, uint16_t count, const char* pattern) {
    // count   = number of LEDs set in the app (1–300)
    // pattern = effect name, e.g. "Solid", "Blink" (or "" if not sent)
    strip.clear();
    if (on) {
      strip.fill(strip.Color(r, g, b), 0, count);
    }
    strip.show();

    Serial.printf("RGB: R=%d G=%d B=%d  On=%s  Count=%d  Pattern=%s\n",
                  r, g, b, on ? "YES" : "NO", count, pattern);
  });
}

void loop() {
  iot.run();
}
```

---

### Timer Widget

The app counts down and sends the remaining seconds to the device. Use it to trigger actions at zero.

App widget: **Timer** on pin `V0`

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

#define BUZZER_PIN 4

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  iot.onTimer("V0", [](float seconds) {
    Serial.println("Timer: " + String((int)seconds) + " s remaining");

    if (seconds <= 0) {
      // Timer finished — sound buzzer
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      Serial.println("Timer done!");
    }
  });
}

void loop() {
  iot.run();
}
```

---

### Joystick Widget

Move the joystick in the app to control direction, speed, or camera pan/tilt on the device.

App widget: **Joystick** on pin `V0`

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

void setup() {
  Serial.begin(115200);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  iot.onJoystick("V0", [](float x, float y) {
    // x and y are -1.0 to 1.0
    // x: left (-1) to right (+1)
    // y: down (-1) to up (+1)

    if (y > 0.5)       Serial.println("Moving FORWARD");
    else if (y < -0.5) Serial.println("Moving BACKWARD");
    else if (x > 0.5)  Serial.println("Turning RIGHT");
    else if (x < -0.5) Serial.println("Turning LEFT");
    else               Serial.println("STOP");

    Serial.printf("  X:%.2f  Y:%.2f\n", x, y);
  });
}

void loop() {
  iot.run();
}
```

---

### State Restoration After Reboot

App commands use **MQTT retain** — when the device reconnects, the broker replays the last command for each widget automatically. No extra code needed for basic restoration.

For safety when the broker restarts, also save state to flash:

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

#define RELAY_PIN 4
bool relayOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  // Restore last state from flash on boot
  relayOn = iot.getBool("relay_on", false);
  digitalWrite(RELAY_PIN, relayOn ? HIGH : LOW);
  Serial.println("Restored relay: " + String(relayOn ? "ON" : "OFF"));

  iot.onSwitch("V0", [](bool on) {
    relayOn = on;
    iot.saveBool("relay_on", on);          // save to flash
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  });
}

void loop() {
  iot.run();
}
```

## OTA Firmware Updates

The library supports automatic Over-The-Air firmware updates through the ThingsLinker Org Portal. The device checks the backend periodically; when a shipment is set to **Live**, the firmware is downloaded and flashed automatically — no USB cable needed.

> **Important — Partition Scheme & Bootloader**
>
> OTA only works when both the **base firmware** and the **new firmware** are compiled with an OTA-capable partition scheme. In Arduino IDE go to:
> **Tools → Partition Scheme → Default with OTA (1.3MB APP / 1.5MB SPIFFS)**
> (or any scheme that includes two OTA app partitions — OTA\_0 and OTA\_1)
>
> - The bootloader is written once when you first flash via USB. After that, OTA updates only replace the app partition — the bootloader and partition table are never touched.
> - If you accidentally compile the new `.bin` with a **different** partition scheme (e.g. "No OTA" or "Huge APP"), the flash will appear to succeed but the device will crash on reboot.
> - If you ever change the partition scheme, you must re-flash the device via USB — OTA cannot update the partition table.
> - **Always use the same partition scheme for every sketch you upload to a device**, including the initial base firmware and every subsequent OTA binary.
>
> **The library performs two automatic pre-flash checks before writing a single byte:**
> 1. **OTA partition check** — calls `esp_ota_get_next_update_partition()`. If it returns `NULL`, the device has no OTA slot (wrong partition scheme) and the update is aborted immediately with a clear Serial message telling you to re-flash via USB.
> 2. **Size check** — compares the firmware file size from the server against the actual OTA partition size. If the new binary is too large (typically because it was compiled with a different scheme), the update is aborted before any bytes are written.
>
> Both failures are reported back to the Org Portal as `failed` with a descriptive error message, so you can see exactly what went wrong in **OTA → Shipments**.

### Org Portal Setup

1. **OTA → New Shipping** — upload your compiled `.bin` and enter a firmware version (e.g. `2.0`)
2. **Set shipment status → Live** — devices detect this on their next `checkOTA()` call
3. Monitor real-time progress in **OTA → Shipments** (pending / in progress / completed / failed per device)

### Basic OTA Sketch

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

static const char*    FIRMWARE_VERSION = "1.0";
static unsigned long  _lastOtaCheck    = 0;
const  unsigned long  OTA_INTERVAL_MS  = 60000UL;  // check every 60 s

void setup() {
  Serial.begin(115200);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();

  if (iot.wifiConnected() && millis() - _lastOtaCheck >= OTA_INTERVAL_MS) {
    _lastOtaCheck = millis();
    Serial.println("[OTA] Checking for update...");

    switch (iot.checkOTA()) {
      case OTA_NO_UPDATE:
        Serial.printf("[OTA] v%s — up to date.\n", FIRMWARE_VERSION);
        break;
      case OTA_FAILED:
        Serial.println("[OTA] Flash failed. Will retry next interval.");
        break;
      case OTA_ERROR:
        Serial.println("[OTA] Server unreachable. Check WiFi / API server.");
        break;
      case OTA_SUCCESS:
        break;  // device restarts inside checkOTA() — this line is never reached
    }
  }
}
```

### OTA Result Codes

| Code | Meaning |
|------|---------|
| `OTA_NO_UPDATE` | No pending update — device is already up to date |
| `OTA_SUCCESS` | Firmware flashed; `ESP.restart()` was called — never returns to caller |
| `OTA_FAILED` | Download or flash failed; failure reported to server; will retry |
| `OTA_ERROR` | Could not reach the server — check `TL_API_SERVER` and WiFi |

### Step-by-Step OTA Workflow

```
[Device v1.0 — 08_OTA_Update]          [Org Portal]
        │                                     │
        │  1. upload 11_OTA_TestFirmware.bin  │
        │     + set shipment Live             │
        │◄────────────────────────────────────┤
        │  2. checkOTA() → has_update: true   │
        │────────────────────────────────────►│ status: in_progress
        │  3. HTTPUpdate downloads .bin       │
        │  4. flash + restart                 │
        │────────────────────────────────────►│ status: completed
        │                                     │
[Device v2.0 — NeoPixel RGB running]
```

### OTA Examples

| Example | Description |
|---------|-------------|
| `08_OTA_Update` | Minimal v1.0 base firmware — OTA check loop only |
| `11_OTA_TestFirmware` | Full v2.0 firmware — NeoPixel RGB + Switch, used as the "new" binary in a shipment |

---

## Storage API

Persist configuration to ESP32 flash (survives reboot):

```cpp
// Save
iot.saveString("name",  "Living Room");
iot.saveInt("threshold", 25);
iot.saveFloat("lat",     37.7749);
iot.saveBool("alarm",    true);

// Load
String name = iot.getString("name", "Unknown");
int thr      = iot.getInt("threshold", 20);
float lat    = iot.getFloat("lat", 0.0);
bool alarm   = iot.getBool("alarm", false);

// Manage
if (iot.hasKey("name"))  iot.removeKey("name");
iot.clearAllData();        // Erases app data, keeps WiFi
```

## Status & Utilities

```cpp
iot.wifiConnected()   // → bool: true if WiFi is connected
iot.mqttConnected()   // → bool: true if MQTT is connected
iot.bleActive()       // → bool: true if BLE provisioning is running
iot.getIP()           // → String: current WiFi IP address
iot.getChipID()       // → String: unique chip ID (from MAC address)
iot.resetWiFi()       // Clear saved WiFi credentials and restart BLE
iot.debug(false)      // Disable library Serial output (default: enabled)
```

### Custom BLE Device Name (`setBLEName`)

By default the device advertises as **`ThingsLinker_XXXXXX`** (last 6 digits of MAC).
If you are building a white-label product, you can change this to your own brand name.

> Call `setBLEName()` **before** `begin()`.

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_AUTH_TOKEN", "YOUR_BLUEPRINT_ID");

void setup() {
  Serial.begin(115200);

  // Change the BLE advertised name to your brand
  iot.setBLEName("SmartHome");
  // Device now appears as: SmartHome_BC6575C55494

  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();
}
```

After uploading, open the ThingsLinker app and scan for BLE devices — your device will appear as `SmartHome_XXXXXX` instead of `ThingsLinker_XXXXXX`.

### Virtual Pin String Tip

When building pin names dynamically use `snprintf`, not `String`:

```cpp
// Correct
char pin[8];
snprintf(pin, sizeof(pin), "V%d", i);
iot.led(pin, state);

// Avoid — String temporary may produce dangling const char*
iot.led("V" + String(i), state);
```

## Widget → Library Function Reference

| App widget | Publish (Device→App) | Subscribe (App→Device) |
|-----------|---------------------|----------------------|
| Button | `iot.button(pin, bool)` | `iot.onButton(pin, cb)` |
| LED | `iot.led(pin, bool)` | `iot.onLED(pin, cb)` |
| Switch | — | `iot.onSwitch(pin, cb)` |
| Slider | `iot.slider(pin, float)` | `iot.onSlider(pin, cb)` |
| Gauge | `iot.gauge(pin, float)` | — |
| Chart | `iot.chart(pin, float)` | — |
| Value Display | `iot.display(pin, float)` | — |
| Label | `iot.label(pin, float)` | — |
| RGB | — | `iot.onRGB(pin, cb)` |
| Timer | — | `iot.onTimer(pin, cb)` |
| Joystick | — | `iot.onJoystick(pin, cb)` |
| Map | `iot.map(pin, lat, lng)` | — |

## MQTT Topic Format

```
device/{WidgetType}/{BlueprintId}/{AuthToken}/{VirtualPin}/
```

Widget type strings (case-sensitive): `Button`, `Switch`, `Slider`, `Gauge`,
`Chart`, `Value Display`, `LED`, `Label`, `RGB`, `Timer`, `Joystick`, `Map`

Payload format:
```json
{"v": 25.5, "t": 1720000000}
```
- `v` — numeric value (float)
- `t` — Unix timestamp (NTP-synced; falls back to millis()/1000 before NTP sync)

Extra fields for complex widgets go at the **root level** of the JSON object:
- RGB: `{"v": 1, "r": 0, "g": 255, "b": 204, "status": "ON", "count": 50, "pattern": "Solid", "t": ts}`
- Map: `{"v": lat, "lat": lat, "lng": lng, "t": ts}`
- Joystick: `{"v": 0, "x": 0.5, "y": -0.3, "t": ts}`

Device status topic (heartbeat + LWT):
```
device/status/{BlueprintId}/{AuthToken}/
Payload: "ONLINE" or "OFFLINE"  (simple string, retained)
```

## BLE Provisioning Protocol

**Service UUID:** `4fafc201-1fb5-459e-8fcc-c5c9c331914b`

| Characteristic | UUID | Direction | Description |
|----------------|------|-----------|-------------|
| WiFi Credentials | `beb5483e-…` | Write | `{"ssid":"…","password":"…"}` |
| Status | `cba1d466-…` | Read/Notify | `{"status":"connected","ip":"…"}` |
| Confirm | `8ec90774-…` | Write | `{"status":"complete"}` triggers restart |

## Examples

Open via **File → Examples → ThingsLinker**:

| Example | Description |
|---------|-------------|
| `00_Connection_Test` | Verify credentials and end-to-end connectivity |
| `01_SimpleExample` | Minimum working sketch (3 lines) |
| `01_BLE_Provisioning` | BLE provisioning + BOOT button re-provisioning |
| `02_BLE_Force_Provision` | Always starts in BLE mode (force re-provisioning) |
| `02_LED_Control` | Control an LED from the app |
| `03_Temperature_Monitor` | Publish sensor readings to Gauge widget |
| `03_Widget_Control_With_Status` | Button + Gauge + online/offline status |
| `04_Advanced_Control` | Multiple widgets simultaneously |
| `05_Storage_Example` | Persist settings across reboots |
| `06_Multiple_Pins` | Using V0–V124 (125 pins) |
| `07_All_Widgets_Test` | All widget types — Button, Switch, Slider, RGB, Timer, Joystick, Gauge, Chart, Value Display, Label, LED |
| `08_ESP32S3_Full_Dashboard` | Full dashboard: NeoPixel RGB, Joystick, Map, Timer, sensors |
| `08_OTA_Update` | **OTA** — v1.0 base firmware with periodic `checkOTA()` loop |
| `09_Clear_WiFi` | Erase credentials & re-provision via BLE |
| `10_Full_Feature_Test` | All publish + subscribe functions with reconnect handling |
| `11_OTA_TestFirmware` | **OTA** — v2.0 NeoPixel RGB firmware; compile this as the "new" `.bin` for OTA shipments |

## Library Architecture

```
src/
├── TL_Config.h         Central config — TL_API_SERVER, broker, pins, timeouts, debug
├── TL_BLE.h/.cpp       BLE provisioning (receive WiFi credentials from app)
├── TL_WiFi.h/.cpp      WiFi connect, persist, reconnect, NTP init
├── TL_MQTT.h/.cpp      TLS MQTT — connect, publish, subscribe, LWT
├── TL_OTA.h/.cpp       OTA firmware update (HTTP download + flash via HTTPUpdate)
├── TL_Storage.h/.cpp   NVS Preferences wrapper
├── TL_Base64.h/.cpp    Base64 encode/decode utility
└── ThingsLinker.h/.cpp Public API — thin wrapper around the above modules
```

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| Device not visible in BLE scan | Restart ESP32; ensure no other client is connected |
| WiFi fails every boot | Check SSID is 2.4 GHz; call `iot.resetWiFi()` to re-provision |
| MQTT state -1 (auth failed) | Verify client key / secret key from portal |
| MQTT state -2 (connect failed) | Check internet connectivity and firewall on port 8883 |
| Timestamps in wrong year | NTP syncs in background — first few payloads may use millis() fallback |
| Sketch won't compile | Install ArduinoJson and PubSubClient via Library Manager |
| OTA returns `OTA_ERROR` | Check `TL_API_SERVER` URL in `TL_Config.h`; ensure device has internet access |
| OTA re-flashes same firmware | Backend auto-corrects stuck `in_progress` records on next `checkOTA()` call |
| OTA returns `OTA_FAILED` | Check Serial for error message; verify `.bin` is valid and shipment is Live |
| OTA keeps looping after flash | The device missed reporting completion — fixed automatically on next boot check |
| Serial: "No OTA partition found" | Base firmware was flashed without OTA partition scheme. Re-flash via USB with Tools → Partition Scheme → Default with OTA |
| Serial: "Firmware too large for OTA partition" | New `.bin` was compiled with a different partition scheme. Recompile with the same scheme as the base firmware and re-upload the `.bin` to the shipment |

## License

MIT — use it however you like.

## Support

- Email: info@thingslinker.com
- Website: https://thingslinker.com
