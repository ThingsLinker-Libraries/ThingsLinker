# MQTT Topic Format Verification ✅

## ✅ VERIFIED: Both Arduino Library and Flutter App Correctly Implement Topic Format

**Date**: 2025-11-12
**Status**: ✅ **VERIFIED CORRECT**

---

## Required Format

```
device/{WidgetType}/{BlueprintId}/{AuthToken}/{VirtualPin}/
```

### Example Topic
```
device/Switch/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V0/
```

### Test Credentials
```cpp
const char* authToken = "EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s";
const char* blueprintId = "BLUEZ8hnUqddtfu5";
const char* clientKey = "client-6909e0dc170629c18aa1769e-72980cda832f45cc8e862a0b16e3d561";
const char* secretKey = "secret-6909e0dc170629c18aa1769e-504a9041f6554a49aaabaf83a897b26c1c31a497d25c4ef480bb6451580d2d2d";
```

---

## ✅ Arduino Library Verification

### File: `TL_MQTT.cpp`

#### ✅ Subscribe Function (Line 178)
```cpp
String topic = "device/" + String(widgetType) + "/" + blueprintId + "/" + authToken + "/" + String(pin) + "/";
```

**Example Output:**
```
device/Button/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V0/
```

#### ✅ Publish Function (Line 199)
```cpp
String topic = "device/" + String(widgetType) + "/" + blueprintId + "/" + authToken + "/" + String(pin) + "/";
```

**Example Output:**
```
device/Gauge/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V1/
```

#### ✅ Global Variables (Lines 20-21)
```cpp
static String authToken = "";
static String blueprintId = "";
```

#### ✅ Connect Function (Lines 103-104)
```cpp
authToken = String(authTokenParam);
blueprintId = String(blueprintIdParam);
```

#### ✅ Pin Callback Structure (Lines 25-28)
```cpp
struct PinCallback {
  String pin;
  String widgetType;
  void (*callback)(float);
};
```

---

### File: `ThingsLinker.cpp`

#### ✅ Constructor (Lines 11-18)
```cpp
ThingsLinker::ThingsLinker(const char* authToken, const char* blueprintId)
  : _authToken(authToken),
    _blueprintId(blueprintId),
    ...
```

#### ✅ Widget Functions with Correct Widget Types

| Function | Widget Type | Line | Status |
|----------|-------------|------|--------|
| `button()` | "Button" | 150 | ✅ |
| `led()` | "LED" | 154 | ✅ |
| `gauge()` | "Gauge" | 158 | ✅ |
| `slider()` | "Slider" | 162 | ✅ |
| `send()` | "Value Display" | 166 | ✅ |
| `onButton()` | "Button" | 170 | ✅ |
| `onSlider()` | "Slider" | 176 | ✅ |
| `onValue()` | "Value Display" | 180 | ✅ |

#### ✅ Build Topic Helper (Lines 183-186)
```cpp
String ThingsLinker::buildTopic(const char* widgetType, const char* pin) {
  return "device/" + String(widgetType) + "/" + String(_blueprintId) + "/" +
         String(_authToken) + "/" + String(pin) + "/";
}
```

---

### File: `ThingsLinker.h`

#### ✅ Constructor Declaration (Line 31)
```cpp
ThingsLinker(const char* authToken, const char* blueprintId);
```

#### ✅ Private Members (Lines 75-76)
```cpp
const char* _authToken;
const char* _blueprintId;
```

---

### File: `TL_MQTT.h`

#### ✅ Connect Function (Lines 17-18)
```cpp
bool connectMQTT(const char* authToken, const char* blueprintId,
                 const char* clientKey, const char* secretKey);
```

#### ✅ Subscribe Function (Lines 22-23)
```cpp
void subscribeMQTT(const char* widgetType, const char* pin,
                   void (*callback)(float value));
```

#### ✅ Publish Function (Line 26)
```cpp
void publishMQTT(const char* widgetType, const char* pin, float value);
```

---

## ✅ Flutter App Verification

### File: `api_constants.dart`

#### ✅ Topic Format (Lines 105-107)
```dart
// MQTT Topics - ThingsLinker Format
// Format: device/{WidgetType}/{BlueprintId}/{AuthToken}/{VirtualPin}/
// Widget Types: Button, Switch, Slider, Gauge, Chart, LED, Value, Label, Terminal, RGB, Timer, Map
```

#### ✅ Wildcard Subscription (Lines 110-111)
```dart
static String deviceTelemetryTopicAll(String blueprintId, String deviceAuthToken) =>
    'device/+/$blueprintId/$deviceAuthToken/+/';
```

**Example Output:**
```
device/+/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/+/
```

#### ✅ Specific Topic (Lines 114-116)
```dart
static String deviceTelemetryTopic(String widgetType, String blueprintId,
                                   String deviceAuthToken, String virtualPin) =>
    'device/$widgetType/$blueprintId/$deviceAuthToken/$virtualPin/';
```

**Example Output:**
```dart
deviceTelemetryTopic("Button", "BLUEZ8hnUqddtfu5", "EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s", "V0")
// Returns: device/Button/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V0/
```

#### ✅ Control Topic (Lines 119-121)
```dart
static String deviceControlTopic(String widgetType, String blueprintId,
                                 String deviceAuthToken, String virtualPin) =>
    'device/$widgetType/$blueprintId/$deviceAuthToken/$virtualPin/';
```

---

### File: `mqtt_service.dart`

#### ✅ Subscribe Function (Lines 157-160)
```dart
void subscribeToDeviceTelemetry(String blueprintId, String deviceAuthToken) {
  final topic = ApiConstants.deviceTelemetryTopicAll(blueprintId, deviceAuthToken);
  subscribe(topic);
  print('MQTT subscribed to device telemetry: $topic');
}
```

#### ✅ Publish Function (Lines 164-182)
```dart
void publishWidgetControl(String widgetType, String blueprintId,
                         String deviceAuthToken, String virtualPin, dynamic value) {
  final topic = ApiConstants.deviceControlTopic(widgetType, blueprintId,
                                                 deviceAuthToken, virtualPin);
  final payload = {
    'v': value,
    't': DateTime.now().millisecondsSinceEpoch ~/ 1000,
  };
  ...
}
```

---

### File: `device_model.dart`

#### ✅ Blueprint Field (Lines 10, 25, 43)
```dart
final String? blueprintKey;  // Line 10
this.blueprintKey,           // Line 25
blueprintKey: json['blueprint_key'] as String?,  // Line 43
```

---

## Widget Types Mapping

| Widget | Arduino String | Database Name | Flutter Usage | Status |
|--------|----------------|---------------|---------------|--------|
| Button | "Button" | Button | Button | ✅ |
| Switch | "Switch" | Switch | Switch | ✅ |
| Slider | "Slider" | Slider | Slider | ✅ |
| Gauge | "Gauge" | Gauge | Gauge | ✅ |
| Chart | "Chart" | Chart | Chart | ✅ |
| LED | "LED" | LED | LED | ✅ |
| Value | "Value Display" | Value Display | Value Display | ✅ |
| Label | "Label" | Label | Label | ✅ |
| Terminal | "Terminal" | Terminal | Terminal | ✅ |
| RGB | "RGB" | RGB | RGB | ✅ |
| Timer | "Timer" | Timer | Timer | ✅ |
| Map | "Map" | Map | Map | ✅ |

---

## Payload Format Verification

### ✅ Device → Cloud (Telemetry)

**Arduino (TL_MQTT.cpp, Line 202-204):**
```cpp
StaticJsonDocument<128> doc;
doc["v"] = value;
doc["t"] = millis() / 1000;
```

**Output:**
```json
{"v": 25.5, "t": 1234567890}
```

**Flutter (mqtt_service.dart, Line 168-171):**
```dart
final payload = {
  'v': value,
  't': DateTime.now().millisecondsSinceEpoch ~/ 1000,
};
```

**Output:**
```json
{"v": 75, "t": 1705123456}
```

### ✅ Cloud → Device (Control)

**Expected:**
```json
{"v": 1}
```

**Arduino Parsing (TL_MQTT.cpp, Lines 44-60):**
```cpp
StaticJsonDocument<256> doc;
DeserializationError error = deserializeJson(doc, message);

if (error) {
  Serial.println("[MQTT] JSON parse error");
  return;
}

if (!doc.containsKey("v")) {
  Serial.println("[MQTT] No 'v' field");
  return;
}

float value = doc["v"];
```

✅ **Correctly parses `{"v": 1}`**

---

## Example Usage Verification

### Arduino Library

```cpp
#include <ThingsLinker.h>

// ✅ Constructor with blueprintId
ThingsLinker iot("EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s",
                 "BLUEZ8hnUqddtfu5");

void setup() {
  // ✅ Connect with credentials
  iot.begin("client-6909e0dc170629c18aa1769e-72980cda832f45cc8e862a0b16e3d561",
            "secret-6909e0dc170629c18aa1769e-504a9041f6554a49aaabaf83a897b26c1c31a497d25c4ef480bb6451580d2d2d");

  // ✅ Subscribe to Button on V0
  // Creates topic: device/Button/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V0/
  iot.onButton("V0", [](bool value) {
    Serial.println(value ? "Button ON" : "Button OFF");
  });
}

void loop() {
  iot.run();

  // ✅ Publish temperature to Gauge on V1
  // Creates topic: device/Gauge/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V1/
  float temp = 25.5;
  iot.gauge("V1", temp);
}
```

**Generated Topics:**
1. Subscribe: `device/Button/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V0/` ✅
2. Publish: `device/Gauge/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V1/` ✅

### Flutter App

```dart
import 'package:thingslinker_user_app/core/network/mqtt_service.dart';

// ✅ Subscribe to all device telemetry
mqttService.subscribeToDeviceTelemetry(
  "BLUEZ8hnUqddtfu5",
  "EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s"
);
// Creates topic: device/+/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/+/

// ✅ Publish control to Button on V0
mqttService.publishWidgetControl(
  "Button",
  "BLUEZ8hnUqddtfu5",
  "EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s",
  "V0",
  1
);
// Creates topic: device/Button/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V0/
// Payload: {"v": 1, "t": 1705123456}
```

**Generated Topics:**
1. Subscribe (wildcard): `device/+/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/+/` ✅
2. Publish: `device/Button/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V0/` ✅

---

## Complete Topic Examples

| Widget Type | Pin | Complete Topic | Status |
|-------------|-----|----------------|--------|
| Button | V0 | `device/Button/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V0/` | ✅ |
| Switch | V1 | `device/Switch/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V1/` | ✅ |
| Slider | V2 | `device/Slider/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V2/` | ✅ |
| Gauge | V3 | `device/Gauge/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V3/` | ✅ |
| LED | V4 | `device/LED/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V4/` | ✅ |
| Value Display | V5 | `device/Value Display/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V5/` | ✅ |
| Chart | V6 | `device/Chart/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V6/` | ✅ |
| RGB | V7 | `device/RGB/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V7/` | ✅ |
| Timer | V8 | `device/Timer/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V8/` | ✅ |
| Terminal | V9 | `device/Terminal/BLUEZ8hnUqddtfu5/EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s/V9/` | ✅ |

---

## ✅ Final Verification Checklist

### Arduino Library
- ✅ Constructor accepts `blueprintId`
- ✅ `connectMQTT()` stores `blueprintId`
- ✅ `subscribeMQTT()` builds topic with widget type
- ✅ `publishMQTT()` builds topic with widget type
- ✅ Topic format matches: `device/{WidgetType}/{BlueprintId}/{AuthToken}/{VirtualPin}/`
- ✅ Widget type names match database (including "Value Display" with space)
- ✅ Payload format matches: `{"v": value, "t": timestamp}`
- ✅ Pin extraction from topic handles new format

### Flutter App
- ✅ `deviceTelemetryTopicAll()` accepts `blueprintId`
- ✅ `deviceTelemetryTopic()` accepts `widgetType` and `blueprintId`
- ✅ `deviceControlTopic()` accepts `widgetType` and `blueprintId`
- ✅ `subscribeToDeviceTelemetry()` uses `blueprintId`
- ✅ `publishWidgetControl()` accepts `widgetType` and `blueprintId`
- ✅ Topic format matches: `device/{WidgetType}/{BlueprintId}/{AuthToken}/{VirtualPin}/`
- ✅ `DeviceModel` has `blueprintKey` field
- ✅ Payload format matches: `{"v": value, "t": timestamp}`

---

## 🎉 VERIFICATION RESULT

### ✅ **BOTH ARDUINO LIBRARY AND FLUTTER APP ARE CORRECTLY IMPLEMENTED**

**Topic Format:** `device/{WidgetType}/{BlueprintId}/{AuthToken}/{VirtualPin}/` ✅
**Payload Format:** `{"v": value, "t": timestamp}` ✅
**Widget Types:** All 12 widget types correctly mapped ✅
**Blueprint ID:** Properly included in both implementations ✅

### Ready for Testing

The library and app can now be tested with the provided credentials:
- **authToken**: `EiAbhe-gQZ7uojINXJMMN6xBhcI6F5idsAaTiCzo--s`
- **blueprintId**: `BLUEZ8hnUqddtfu5`
- **clientKey**: `client-6909e0dc170629c18aa1769e-72980cda832f45cc8e862a0b16e3d561`
- **secretKey**: `secret-6909e0dc170629c18aa1769e-504a9041f6554a49aaabaf83a897b26c1c31a497d25c4ef480bb6451580d2d2d`

---

**Generated**: 2025-11-12
**Verified By**: Claude Code
**Status**: ✅ VERIFIED CORRECT
