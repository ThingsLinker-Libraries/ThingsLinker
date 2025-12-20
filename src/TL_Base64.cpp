/**
 * @file TL_Base64.cpp
 * @brief Implementation of Base64 encoding/decoding
 */

#include "TL_Base64.h"

// Base64 characters
static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

/**
 * @brief Encode string to Base64
 */
String base64Encode(const String& input) {
  String output = "";
  int val = 0;
  int valb = -6;

  for (unsigned char c : input) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      output += base64_chars[(val >> valb) & 0x3F];
      valb -= 6;
    }
  }

  if (valb > -6) {
    output += base64_chars[((val << 8) >> (valb + 8)) & 0x3F];
  }

  while (output.length() % 4) {
    output += '=';
  }

  return output;
}

/**
 * @brief Decode Base64 string
 */
String base64Decode(const String& input) {
  String output = "";
  int val = 0;
  int valb = -8;

  for (unsigned char c : input) {
    if (c == '=') break;

    // Find character index
    const char* p = strchr(base64_chars, c);
    if (!p) continue;

    val = (val << 6) + (p - base64_chars);
    valb += 6;

    if (valb >= 0) {
      output += char((val >> valb) & 0xFF);
      valb -= 8;
    }
  }

  return output;
}
