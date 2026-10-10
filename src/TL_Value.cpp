/**
 * @file TL_Value.cpp
 * @brief Typed widget value conversions
 */

#include "TL_Value.h"

long long TLValue::asInt() const {
  switch (_kind) {
    case K_BOOL:   return _u.b ? 1 : 0;
    case K_INT:    return _u.i;
    case K_FLOAT:  return (long long)_u.f;
    case K_DOUBLE: return (long long)_u.d;
    default:       return atoll(_u.s);
  }
}

double TLValue::asDouble() const {
  switch (_kind) {
    case K_BOOL:   return _u.b ? 1.0 : 0.0;
    case K_INT:    return (double)_u.i;
    case K_FLOAT:  return (double)_u.f;
    case K_DOUBLE: return _u.d;
    default:       return atof(_u.s);
  }
}

bool TLValue::asBool() const {
  switch (_kind) {
    case K_BOOL:   return _u.b;
    case K_INT:    return _u.i != 0;
    case K_FLOAT:  return _u.f != 0.0f;
    case K_DOUBLE: return _u.d != 0.0;
    default:       return _u.s[0] != '\0';
  }
}

String TLValue::toString() const {
  switch (_kind) {
    case K_BOOL:   return String(_u.b ? "true" : "false");
    case K_INT: {
      char buf[24];
      snprintf(buf, sizeof(buf), "%lld", _u.i);
      return String(buf);
    }
    case K_FLOAT:  return String(_u.f);
    case K_DOUBLE: return String(_u.d);
    default:       return String(_u.s);
  }
}
