/**
 * @file TL_Base64.h
 * @brief Simple Base64 encoding/decoding
 *
 * Easy functions for Base64 conversion
 */

#ifndef TL_BASE64_H
#define TL_BASE64_H

#include <Arduino.h>

/**
 * @brief Encode string to Base64
 * @param input String to encode
 * @return Base64 encoded string
 */
String base64Encode(const String& input);

/**
 * @brief Decode Base64 string
 * @param input Base64 string
 * @return Decoded string
 */
String base64Decode(const String& input);

#endif // TL_BASE64_H
