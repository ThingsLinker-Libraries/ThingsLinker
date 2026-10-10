/**
 * @file TL_Value.h
 * @brief Typed widget value — the single place where data types are decided
 *
 * A TLValue carries one widget value together with its data type, so the same
 * publish call works for every type the platform supports:
 *
 *   iot.display("V1", 42);          // Integer
 *   iot.display("V1", 23.5f);       // Float
 *   iot.display("V1", true);        // Boolean
 *   iot.display("V1", "Running");   // String
 *
 * The type is taken from the C++ argument type and never guessed from the
 * content, so the string "123" stays a string and the integer 123 stays an
 * integer all the way to the dashboard.
 *
 * Data types match the widget "Data Type" setting in the portal:
 *
 *   TL_INT     → Integer   {"v": 42}
 *   TL_FLOAT   → Float     {"v": 23.5}
 *   TL_BOOL    → Boolean   {"v": 1} / {"v": 0}
 *   TL_STRING  → String    {"v": "Running"}
 *
 * A TLValue holding a string only borrows the pointer. It is meant to be
 * created as a function argument and used immediately, not stored.
 */

#ifndef TL_VALUE_H
#define TL_VALUE_H

#include <Arduino.h>

enum TLType : uint8_t {
  TL_INT,
  TL_FLOAT,
  TL_BOOL,
  TL_STRING
};

class TLValue {
public:
  // One constructor per C++ type, the same set Arduino's Print uses, so any
  // argument picks its own type without ambiguity.
  TLValue(bool v)               : _kind(K_BOOL)   { _u.b = v; }
  TLValue(int v)                : _kind(K_INT)    { _u.i = v; }
  TLValue(unsigned int v)       : _kind(K_INT)    { _u.i = v; }
  TLValue(long v)               : _kind(K_INT)    { _u.i = v; }
  TLValue(unsigned long v)      : _kind(K_INT)    { _u.i = (long long)v; }
  TLValue(long long v)          : _kind(K_INT)    { _u.i = v; }
  TLValue(unsigned long long v) : _kind(K_INT)    { _u.i = (long long)v; }
  TLValue(float v)              : _kind(K_FLOAT)  { _u.f = v; }
  TLValue(double v)             : _kind(K_DOUBLE) { _u.d = v; }
  TLValue(const char* v)        : _kind(K_STRING) { _u.s = v ? v : ""; }
  TLValue(const String& v)      : _kind(K_STRING) { _u.s = v.c_str(); }

#ifdef ESP32
  // F("...") strings are ordinary pointers on ESP32.
  TLValue(const __FlashStringHelper* v) : _kind(K_STRING) {
    _u.s = v ? reinterpret_cast<const char*>(v) : "";
  }
#else
  // Without this, F("...") would silently convert to the bool overload.
  TLValue(const __FlashStringHelper* v) = delete;
#endif

  TLType type() const {
    switch (_kind) {
      case K_BOOL:   return TL_BOOL;
      case K_INT:    return TL_INT;
      case K_STRING: return TL_STRING;
      default:       return TL_FLOAT;
    }
  }

  bool isNumber() const { return _kind == K_INT || _kind == K_FLOAT || _kind == K_DOUBLE; }
  bool isString() const { return _kind == K_STRING; }
  bool isBool()   const { return _kind == K_BOOL; }

  /** true when the value was given as a float (not a double); keeps short float formatting on the wire */
  bool isSinglePrecision() const { return _kind == K_FLOAT; }

  long long   asInt()    const;
  double      asDouble() const;
  float       asFloat()  const { return (float)asDouble(); }
  bool        asBool()   const;
  /** String content, or "" when the value is not a string. */
  const char* asString() const { return _kind == K_STRING ? _u.s : ""; }

  /** Human readable form for debug logs. */
  String toString() const;

private:
  enum Kind : uint8_t { K_BOOL, K_INT, K_FLOAT, K_DOUBLE, K_STRING };

  Kind _kind;
  union {
    bool        b;
    long long   i;
    float       f;
    double      d;
    const char* s;
  } _u;
};

#endif // TL_VALUE_H
