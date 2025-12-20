# ThingsLinker - Super Easy IoT Library for ESP32

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/thingslinker/arduino-library)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-ESP32-orange.svg)](https://www.espressif.com/en/products/socs/esp32)

**So easy that even a 10-year-old can use it!** 🚀

Connect your ESP32 to the ThingsLinker IoT platform with just **3 lines of code**. No complex setup, no WiFi credentials in code, no hassle. Just simple, clean, and it works!

## ✨ Features

- 🔵 **BLE Provisioning** - Configure WiFi via mobile app (no hardcoded credentials!)
- 📶 **Auto WiFi Connection** - Automatic WiFi connection with persistent storage
- 🔄 **Auto Reconnection** - Automatic WiFi and MQTT reconnection
- 💬 **MQTT Communication** - Real-time bidirectional communication
- 🎛️ **Simple Widgets** - Button, LED, Slider, Gauge, and more
- 📱 **Mobile App Integration** - Works seamlessly with ThingsLinker mobile app
- 🧩 **Modular Design** - Clean, separated code for easy understanding

## 🚀 Quick Start

### Installation

1. Download this library
2. In Arduino IDE: **Sketch** → **Include Library** → **Add .ZIP Library**
3. Select the downloaded file
4. Done!

### Dependencies

This library requires:
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) (v6.x or higher)
- [PubSubClient](https://github.com/knolleary/pubsubclient) (v2.8 or higher)

Install them via Arduino Library Manager.

### Your First Program

```cpp
#include <ThingsLinker.h>

// Create ThingsLinker object with your device token
ThingsLinker iot("YOUR_DEVICE_AUTH_TOKEN");

void setup() {
  // Initialize with your credentials
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  // That's it! Just call run()
  iot.run();
}
```

**Done!** 🎉 Your device is now connected to ThingsLinker!

## 📱 How It Works

### First Time Setup (BLE Provisioning)

1. Upload code to ESP32
2. ESP32 starts BLE automatically (name: `ThingsLinker_XXXXXX`)
3. Open ThingsLinker mobile app
4. Tap "Add Device" and scan for your device
5. Enter your WiFi credentials
6. Device connects to WiFi
7. BLE stops automatically
8. MQTT connects
9. **Device ready!** ✅

### Next Time

1. Device remembers WiFi credentials
2. Auto-connects to WiFi
3. Auto-connects to MQTT
4. **Device ready in seconds!** ⚡

## 🎮 Control Your Device

### Button Control

Control an LED from the mobile app:

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_DEVICE_AUTH_TOKEN");
const int LED_PIN = 2;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");

  // Listen for button on pin V0
  iot.onButton("V0", [](bool value) {
    digitalWrite(LED_PIN, value ? HIGH : LOW);
  });
}

void loop() {
  iot.run();
}
```

### Send Sensor Data

Send temperature to a gauge widget:

```cpp
#include <ThingsLinker.h>

ThingsLinker iot("YOUR_DEVICE_AUTH_TOKEN");

void setup() {
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}

void loop() {
  iot.run();

  // Send temperature every 5 seconds
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    lastSend = millis();

    float temperature = 25.5;  // Your sensor reading
    iot.gauge("V0", temperature);
  }
}
```

## 📚 Widget Functions

**125 Virtual Pins Supported**: V0 to V124 - More than enough for any project! 🎉

### Control Widgets (Receive from App)

```cpp
iot.onButton("V0", callback);    // Button press (true/false)
iot.onSlider("V1", callback);    // Slider value (float)
iot.onValue("V2", callback);     // Any value (float)

// Use any pin from V0 to V124!
iot.onButton("V124", callback);  // Last pin works too!
```

### Display Widgets (Send to App)

```cpp
iot.button("V0", true);          // Button state
iot.led("V1", true);             // LED state
iot.slider("V2", 50.0);          // Slider value
iot.gauge("V3", 25.5);           // Gauge value
iot.send("V4", 123.45);          // Any value

// Use any pin from V0 to V124!
iot.gauge("V124", 99.9);         // All 125 pins available!
```

## 💾 Storage Functions (Preferences)

Save and retrieve data from the mobile app using ESP32 flash memory:

```cpp
// Save data from app
iot.saveString("deviceName", "Living Room");
iot.saveInt("threshold", 25);
iot.saveFloat("temperature", 25.5);
iot.saveBool("alarmActive", true);

// Read stored data (survives reboot!)
String name = iot.getString("deviceName", "Unnamed");
int threshold = iot.getInt("threshold", 20);
float temp = iot.getFloat("temperature", 0.0);
bool active = iot.getBool("alarmActive", false);

// Check and manage
if (iot.hasKey("deviceName")) {
  iot.removeKey("deviceName");  // Remove specific key
}
iot.clearAllData();  // Clear all (WiFi safe!)
```

**Example Use Case**: Save device configuration, thresholds, names, or any settings from the app!

## 🔧 Utility Functions

```cpp
// Status checks
bool wifiOk = iot.wifiConnected();   // WiFi connected?
bool mqttOk = iot.mqttConnected();   // MQTT connected?
bool bleOn = iot.bleActive();        // BLE active?

// Get information
String ip = iot.getIP();             // Device IP address
String id = iot.getChipID();         // Unique chip ID

// Reset WiFi (re-enable BLE provisioning)
iot.resetWiFi();

// Enable/disable debug output
iot.debug(true);
```

## 📖 Examples

The library includes **7 complete examples**:

0. **00_HiveMQ_Test** - Test without backend (HiveMQ broker) 🆕
1. **01_SimpleExample** - Absolute basics (start here!)
2. **02_LED_Control** - Control LED from app
3. **03_Temperature_Monitor** - Send sensor data
4. **04_Advanced_Control** - Multiple widgets
5. **05_Storage_Example** - Save app data to flash memory
6. **06_Multiple_Pins** - Using many pins (V0-V124 demo)

Find them in: **File** → **Examples** → **ThingsLinker**

## 🔑 Getting Your Credentials

1. Go to [ThingsLinker Portal](https://portal.thingslinker.com)
2. Create an account (Organization)
3. Create a device
4. Copy these credentials:
   - **Device Auth Token** (e.g., `EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s`)
   - **Client Key** (e.g., `client-xxxxx-xxxxx`)
   - **Secret Key** (e.g., `secret-xxxxx-xxxxx`)
5. Paste them in your code

## 🏗️ Library Architecture

```
ThingsLinker/src/
├── TL_Config.h          - Configuration (MQTT server, BLE UUIDs, etc.)
├── TL_Base64.h/cpp      - Base64 encoding/decoding
├── TL_BLE.h/cpp         - BLE provisioning module
├── TL_WiFi.h/cpp        - WiFi management module
├── TL_MQTT.h/cpp        - MQTT communication module
├── TL_Storage.h/cpp     - Preferences storage module (NEW!)
└── ThingsLinker.h/cpp   - Main simple wrapper (what you use!)
```

Each module is separate and easy to understand. Perfect for learning!

**New in v1.0.0**: TL_Storage module for saving app data to ESP32 flash memory using Preferences API!

## 🔌 MQTT Topics

The library uses simplified MQTT topics:

### Device Subscribes (Control from App):
```
thingslinker/{AUTH_TOKEN}/sub/{PIN}
```

Example: `thingslinker/EiAbhe.../sub/V0`

### Device Publishes (Telemetry to App):
```
thingslinker/{AUTH_TOKEN}/pub/{PIN}
```

Example: `thingslinker/EiAbhe.../pub/V1`

### Payload Format:
```json
{
  "v": 25.5,
  "t": 1234567890
}
```

- `v`: Value (required)
- `t`: Timestamp in seconds (optional)

## 🔵 BLE Provisioning Protocol

The library uses BLE GATT for provisioning:

**Service UUID**: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`

**Characteristics**:
- **WiFi Credentials** (Write): `beb5483e-36e1-4688-b7f5-ea07361b26a8`
- **Status** (Read + Notify): `cba1d466-344c-4be3-ab3f-189f80dd7518`

**WiFi Credentials Format** (JSON):
```json
{
  "ssid": "YourWiFi",
  "password": "YourPassword"
}
```

**Status Response** (JSON):
```json
{
  "status": "connected",
  "ip": "192.168.1.100"
}
```

## 🧪 Testing with HiveMQ (No Backend Required)

Want to test the library without setting up the ThingsLinker backend? Use HiveMQ public broker!

### Quick Setup:

1. **Edit** `ThingsLinker/src/TL_Config.h`:
   ```cpp
   // Comment out production server:
   // #define MQTT_SERVER "mqtt.thingslinker.com"

   // Uncomment HiveMQ server:
   #define MQTT_SERVER "broker.hivemq.com"
   ```

2. **Upload** the `00_HiveMQ_Test` example

3. **Test with MQTT Client**:
   - **Desktop**: MQTT Explorer or MQTT.fx
   - **Web**: http://www.hivemq.com/demos/websocket-client/
   - **CLI**: `mosquitto_pub` / `mosquitto_sub`

4. **Control Your Device**:
   ```bash
   # Turn LED ON
   mosquitto_pub -h broker.hivemq.com -t "thingslinker/YOUR_DEVICE_ID/sub/V0" -m '{"v":1}'

   # Turn LED OFF
   mosquitto_pub -h broker.hivemq.com -t "thingslinker/YOUR_DEVICE_ID/sub/V0" -m '{"v":0}'
   ```

⚠️ **Note**: HiveMQ is public - use only for testing!

## ❓ FAQ

### Q: Do I need to hardcode WiFi credentials?
**A:** No! Use BLE provisioning via the mobile app.

### Q: What if I change my WiFi password?
**A:** Call `iot.resetWiFi()` to reconfigure.

### Q: Can I use multiple devices?
**A:** Yes! Each device gets a unique auth token.

### Q: What ESP32 boards are supported?
**A:** All ESP32 boards with BLE support (ESP32, ESP32-S3, ESP32-C3, etc.)

### Q: Can I test without ThingsLinker backend?
**A:** Yes! Use HiveMQ public broker. See "Testing with HiveMQ" section above.

### Q: What happens if WiFi disconnects?
**A:** Library auto-reconnects every 5 seconds.

### Q: What happens if MQTT disconnects?
**A:** Library auto-reconnects every 5 seconds.

### Q: Can I see debug output?
**A:** Yes! Open Serial Monitor (115200 baud). You'll see connection status, messages, etc.

## 🐛 Troubleshooting

### Device not showing in BLE scan

- Make sure no other device is connected to the ESP32 BLE
- Restart ESP32
- Check Serial Monitor for BLE startup message

### WiFi not connecting

- Check WiFi credentials are correct
- Make sure WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- Check Serial Monitor for error messages

### MQTT not connecting

- Verify your credentials are correct
- Check internet connection
- Make sure MQTT server is reachable
- Check Serial Monitor for MQTT status

### Library not compiling

- Install dependencies: ArduinoJson, PubSubClient
- Select correct board: ESP32 Dev Module (or your specific board)
- Make sure ESP32 board support is installed

## 📄 License

MIT License - Use it however you want!

## 🤝 Support

- 📧 Email: info@thingslinker.com
- 🌐 Website: https://thingslinker.com
- 📱 Download App: [Google Play](#) | [App Store](#)

## 🎯 Made With ❤️

This library is designed to be **super easy** for everyone - from beginners to professionals. If you find it useful, please ⭐ star the repo!

---

**Happy IoT Building!** 🚀
