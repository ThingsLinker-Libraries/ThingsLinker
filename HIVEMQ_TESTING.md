# HiveMQ Testing Support

## Overview

The ThingsLinker library now supports testing with **HiveMQ public MQTT broker** (`broker.hivemq.com:1883`). This allows you to test the library without setting up the full ThingsLinker backend infrastructure!

## 🎯 Benefits

✅ **No Backend Required** - Test without ThingsLinker server
✅ **Quick Testing** - Start testing in minutes
✅ **Hardware Verification** - Test ESP32 hardware and connectivity
✅ **MQTT Learning** - Learn MQTT protocol basics
✅ **Development** - Rapid prototyping and debugging
✅ **Free** - HiveMQ public broker is free to use

## ⚙️ Setup Instructions

### Step 1: Edit TL_Config.h

Open `ThingsLinker/src/TL_Config.h` and make these changes:

```cpp
// ========== MQTT Server Configuration ==========
// Production MQTT server (COMMENT OUT for testing)
// #define MQTT_SERVER "mqtt.thingslinker.com"
// #define MQTT_PORT 1883

// ========== Test MQTT Server (HiveMQ Public Broker) ==========
// For testing without ThingsLinker backend (UNCOMMENT for testing)
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
```

### Step 2: Upload Test Sketch

Upload the **00_HiveMQ_Test** example:
- Arduino IDE: **File** → **Examples** → **ThingsLinker** → **00_HiveMQ_Test**
- Open Serial Monitor (115200 baud)
- Note the Device ID shown (e.g., `test_device_1234`)

### Step 3: Choose Testing Tool

Pick one of these MQTT clients to control your device:

## 🛠️ Testing Tools

### Option 1: MQTT Explorer (Recommended)

**Best for:** Visual testing, beginners

1. Download: http://mqtt-explorer.com/
2. Launch MQTT Explorer
3. **Connect**:
   - Protocol: `mqtt://`
   - Host: `broker.hivemq.com`
   - Port: `1883`
4. **Subscribe**: `thingslinker/test_device_XXXX/#`
5. **Publish**:
   - Topic: `thingslinker/test_device_XXXX/sub/V0`
   - Message: `{"v": 1}` (LED ON)
   - Message: `{"v": 0}` (LED OFF)

### Option 2: HiveMQ Web Client

**Best for:** No installation needed

1. Go to: http://www.hivemq.com/demos/websocket-client/
2. Click **Connect**
3. Add subscription: `thingslinker/test_device_XXXX/#`
4. Publish messages to control LED

### Option 3: mosquitto_pub/sub (CLI)

**Best for:** Scripting, automation

```bash
# Install mosquitto (if not installed)
# macOS: brew install mosquitto
# Ubuntu: sudo apt-get install mosquitto-clients
# Windows: Download from https://mosquitto.org/download/

# Subscribe to all device topics (receive data)
mosquitto_sub -h broker.hivemq.com -t "thingslinker/test_device_XXXX/#" -v

# Control LED ON
mosquitto_pub -h broker.hivemq.com \
  -t "thingslinker/test_device_XXXX/sub/V0" \
  -m '{"v":1}'

# Control LED OFF
mosquitto_pub -h broker.hivemq.com \
  -t "thingslinker/test_device_XXXX/sub/V0" \
  -m '{"v":0}'

# Slider control (0-255 for brightness)
mosquitto_pub -h broker.hivemq.com \
  -t "thingslinker/test_device_XXXX/sub/V2" \
  -m '{"v":128}'
```

### Option 4: MQTT.fx

**Best for:** Desktop GUI, advanced features

1. Download: https://mqttfx.jensd.de/
2. Create connection profile for `broker.hivemq.com:1883`
3. Subscribe to: `thingslinker/test_device_XXXX/pub/#`
4. Publish to: `thingslinker/test_device_XXXX/sub/V0`

## 📊 Test Topics

Replace `test_device_XXXX` with your actual device ID from Serial Monitor.

### Control Topics (Publish to ESP32)
```
thingslinker/test_device_XXXX/sub/V0   - Button control (LED on/off)
thingslinker/test_device_XXXX/sub/V2   - Slider control (0-255)
```

**Payload Format**:
```json
{"v": 1}      // Value 1 (LED ON)
{"v": 0}      // Value 0 (LED OFF)
{"v": 128}    // Value 128 (half brightness)
```

### Telemetry Topics (Subscribe from ESP32)
```
thingslinker/test_device_XXXX/pub/V1   - LED status (0 or 1)
thingslinker/test_device_XXXX/pub/V3   - Temperature reading
```

**Payload Format** (from ESP32):
```json
{
  "v": 25.5,           // Value
  "t": 1234567890      // Timestamp (optional)
}
```

## 🧪 Test Scenarios

### Test 1: LED Control
```bash
# Turn LED ON
mosquitto_pub -h broker.hivemq.com \
  -t "thingslinker/test_device_1234/sub/V0" \
  -m '{"v":1}'

# Expected: LED turns ON, Serial shows "Button V0: ON"
```

### Test 2: LED Brightness
```bash
# Set brightness to 50%
mosquitto_pub -h broker.hivemq.com \
  -t "thingslinker/test_device_1234/sub/V2" \
  -m '{"v":128}'

# Expected: LED dims to 50%, Serial shows "Slider V2: 128"
```

### Test 3: Receive Temperature
```bash
# Subscribe to temperature
mosquitto_sub -h broker.hivemq.com \
  -t "thingslinker/test_device_1234/pub/V3" \
  -v

# Expected: Receive temperature every 5 seconds
# Output: thingslinker/test_device_1234/pub/V3 {"v":25.5,"t":1234567890}
```

### Test 4: Bidirectional Communication
```bash
# Terminal 1: Subscribe to all
mosquitto_sub -h broker.hivemq.com \
  -t "thingslinker/test_device_1234/#" \
  -v

# Terminal 2: Control LED
mosquitto_pub -h broker.hivemq.com \
  -t "thingslinker/test_device_1234/sub/V0" \
  -m '{"v":1}'

# Expected: See LED status published to V1
```

## 🔧 Code Changes for HiveMQ

The library automatically detects HiveMQ mode:

```cpp
// MQTT connection handles authentication automatically
bool connectMQTT(const char* authToken, const char* clientKey, const char* secretKey) {
  if (strlen(clientKey) > 0 && strlen(secretKey) > 0) {
    // Production mode - with authentication
    mqttClient.connect(clientId.c_str(), clientKey, secretKey);
  } else {
    // Test mode - without authentication (HiveMQ)
    mqttClient.connect(clientId.c_str());
  }
}
```

## ⚠️ Important Notes

### Security Warnings

1. **Public Broker**: HiveMQ is PUBLIC - anyone can subscribe to your topics!
2. **No Encryption**: Data is sent in plain text
3. **No Authentication**: No username/password required
4. **No Privacy**: Don't send sensitive data
5. **Testing Only**: Use only for development and testing

### Limitations

- ❌ No data persistence
- ❌ No message history
- ❌ No QoS guarantees
- ❌ Topics not private
- ❌ Anyone can publish to your topics

### Best Practices

✅ Use random device IDs (library does this automatically)
✅ Test basic functionality only
✅ Don't send personal or sensitive data
✅ Switch to production broker for real applications

## 🔄 Switching Back to Production

After testing, switch back to production:

### Step 1: Edit TL_Config.h

```cpp
// ========== MQTT Server Configuration ==========
// Production MQTT server (UNCOMMENT for production)
#define MQTT_SERVER "mqtt.thingslinker.com"
#define MQTT_PORT 1883

// ========== Test MQTT Server (HiveMQ Public Broker) ==========
// For testing without ThingsLinker backend (COMMENT OUT for production)
// #define MQTT_SERVER "broker.hivemq.com"
// #define MQTT_PORT 1883
```

### Step 2: Use Real Credentials

```cpp
// Production credentials
ThingsLinker iot("YOUR_REAL_AUTH_TOKEN");

void setup() {
  iot.begin("YOUR_CLIENT_KEY", "YOUR_SECRET_KEY");
}
```

## 🐛 Troubleshooting

### ESP32 Not Connecting

**Problem**: Serial shows `[MQTT] ✗ Connection failed`

**Solutions**:
1. Verify TL_Config.h is edited correctly
2. Make sure `MQTT_SERVER` is `"broker.hivemq.com"`
3. Save file and re-upload sketch
4. Check WiFi connection first
5. Try restarting ESP32

### Not Seeing Messages

**Problem**: MQTT client not receiving messages

**Solutions**:
1. Check device ID matches exactly (case-sensitive)
2. Verify topic format: `thingslinker/device_id/sub/V0`
3. Make sure you're subscribed to correct topic
4. Try subscribing to `#` (all topics) first
5. Check Serial Monitor for debug messages

### LED Not Responding

**Problem**: Publishing messages but LED not changing

**Solutions**:
1. Verify JSON format: `{"v": 1}` or `{"v": 0}`
2. Check topic ends with `/sub/V0` (not `/pub/V0`)
3. Make sure ESP32 is connected (check Serial)
4. Try different LED pin if built-in LED doesn't work
5. Check LED polarity (some boards are inverted)

### Connection Drops

**Problem**: ESP32 disconnects randomly

**Solutions**:
1. Check WiFi signal strength
2. Verify router stability
3. Try different WiFi channel
4. Increase MQTT keepalive: `#define MQTT_KEEPALIVE 120`
5. Check power supply (weak power can cause resets)

## 📈 Performance

HiveMQ public broker performance:

- **Latency**: 50-200ms (varies by location)
- **Throughput**: Limited by internet connection
- **Reliability**: Best-effort (no guarantees)
- **Availability**: Usually 99%+ uptime

For production, use `mqtt.thingslinker.com` with better:
- ✅ Lower latency
- ✅ Higher reliability
- ✅ Data persistence
- ✅ Security & privacy
- ✅ QoS support

## 🎓 Learning Resources

- **MQTT Basics**: https://mqtt.org/
- **HiveMQ Docs**: https://www.hivemq.com/docs/
- **MQTT Explorer**: http://mqtt-explorer.com/
- **mosquitto**: https://mosquitto.org/

## ✅ Summary

HiveMQ testing support allows you to:

1. ✅ Test library without backend setup
2. ✅ Verify hardware functionality quickly
3. ✅ Learn MQTT protocol basics
4. ✅ Rapid prototyping and debugging
5. ✅ Easy development workflow

Perfect for getting started, but remember to switch to production broker for real applications!

---

**Happy Testing!** 🚀
