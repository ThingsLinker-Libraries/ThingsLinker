/**
 * @file TL_Codec.h
 * @brief MQTT payload encoding/decoding shared by the WiFi and GSM transports
 *
 * Wire format (identical on the device, backend, portals and mobile app):
 *
 *   {"v": <value>, "t": <unix_timestamp>}
 *
 * "v" carries the value in its native JSON type:
 *   Integer → 42       Float → 23.5       Boolean → 1 / 0       String → "text"
 *
 * Booleans travel as 1/0 so numeric consumers (alerts, charts, firmware
 * written against earlier library versions) keep working.
 *
 * Decoding is tolerant: a number, a boolean, or a numeric string is accepted
 * wherever a number is expected, and text may arrive either as a string "v"
 * or in the legacy root-level "text" field.
 */

#ifndef TL_CODEC_H
#define TL_CODEC_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "TL_Value.h"

#if ARDUINOJSON_VERSION_MAJOR < 7
  #error "ThingsLinker requires ArduinoJson 7 or newer. Update it in the Arduino Library Manager."
#endif

/**
 * Serialise {"v": value, "t": timestamp} into out.
 * @return number of bytes written, or 0 if the payload does not fit.
 */
size_t tlEncodePayload(const TLValue& value, unsigned long timestamp, char* out, size_t outSize);

/** Numeric view of "v" (bool → 1/0, numeric string → parsed, other text → 0). */
float tlNumberOf(const JsonDocument& doc);

/** Boolean view of "v" (number > 0, true, "true", "on"). */
bool tlBoolOf(const JsonDocument& doc);

/**
 * Text view of the message: string "v", else the legacy "text" field, else
 * "v" rendered as text into scratch. Never returns null. The returned pointer
 * is valid only while doc and scratch are.
 */
const char* tlTextOf(const JsonDocument& doc, char* scratch, size_t scratchSize);

#endif // TL_CODEC_H
