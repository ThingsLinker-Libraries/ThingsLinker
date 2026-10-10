/**
 * @file TL_Codec.cpp
 * @brief MQTT payload encoding/decoding shared by the WiFi and GSM transports
 */

#include "TL_Codec.h"
#include <ctype.h>

size_t tlEncodePayload(const TLValue& value, unsigned long timestamp, char* out, size_t outSize) {
  if (!out || outSize == 0) return 0;

  JsonDocument doc;
  switch (value.type()) {
    case TL_BOOL:   doc["v"] = value.asBool() ? 1 : 0; break;
    case TL_INT:    doc["v"] = value.asInt();          break;
    case TL_STRING: doc["v"] = value.asString();       break;
    default:
      if (value.isSinglePrecision()) doc["v"] = value.asFloat();
      else                           doc["v"] = value.asDouble();
      break;
  }
  doc["t"] = timestamp;

  // serializeJson() truncates silently, so check the size first.
  if (measureJson(doc) + 1 > outSize) return 0;
  return serializeJson(doc, out, outSize);
}

static bool equalsIgnoreCase(const char* a, const char* b) {
  while (*a && *b) {
    if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return false;
    a++;
    b++;
  }
  return *a == '\0' && *b == '\0';
}

float tlNumberOf(const JsonDocument& doc) {
  JsonVariantConst v = doc["v"];

  if (v.is<bool>())  return v.as<bool>() ? 1.0f : 0.0f;
  if (v.is<float>()) return v.as<float>();

  if (v.is<const char*>()) {
    const char* s = v.as<const char*>();
    if (!s) return 0.0f;
    while (*s == ' ') s++;
    char* end = nullptr;
    float n = strtof(s, &end);
    if (end != s) return n;
    if (equalsIgnoreCase(s, "true") || equalsIgnoreCase(s, "on")) return 1.0f;
  }
  return 0.0f;
}

bool tlBoolOf(const JsonDocument& doc) {
  return tlNumberOf(doc) > 0.0f;
}

const char* tlTextOf(const JsonDocument& doc, char* scratch, size_t scratchSize) {
  JsonVariantConst v = doc["v"];
  if (v.is<const char*>()) {
    const char* s = v.as<const char*>();
    return s ? s : "";
  }

  JsonVariantConst text = doc["text"];
  if (text.is<const char*>()) {
    const char* s = text.as<const char*>();
    return s ? s : "";
  }

  if (!scratch || scratchSize == 0) return "";
  scratch[0] = '\0';
  if (!v.isNull()) serializeJson(v, scratch, scratchSize);
  return scratch;
}
