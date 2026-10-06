/**
 * @file TL_OTA.h
 * @brief Over-The-Air firmware update for ThingsLinker devices
 *
 * Checks the ThingsLinker backend for a pending firmware shipment and,
 * if one is found, downloads and flashes it using the ESP32 HTTPUpdate library.
 *
 * Update flow
 * -----------
 *   1. GET /api/v1/org/ota/device/check/   → has_update, firmware_url, ota_device_id
 *   2. PUT /api/v1/org/ota/device/update-status/  { status: "in_progress" }
 *   3. HTTPUpdate downloads and flashes the .bin from firmware_url
 *   4. PUT update-status  { status: "completed" }  — then ESP.restart()
 *      OR
 *      PUT update-status  { status: "failed", error_message: "..." }
 *
 * Authentication
 * --------------
 * All requests carry the header:
 *   X-Device-Token: <device auth_token>
 *
 * Usage (standalone)
 * ------------------
 *   #include "TL_OTA.h"
 *   OTAResult r = checkAndApplyOTA("YOUR_AUTH_TOKEN", "http://192.168.1.10:8000");
 *
 * Usage (via ThingsLinker class)
 * ------------------------------
 *   OTAResult r = iot.checkOTA("http://192.168.1.10:8000");
 *   // or use the default TL_API_SERVER:
 *   OTAResult r = iot.checkOTA();
 */

#ifndef TL_OTA_H
#define TL_OTA_H

#include <Arduino.h>

/**
 * Result codes returned by checkAndApplyOTA() / iot.checkOTA().
 */
enum OTAResult {
  OTA_NO_UPDATE = 0,  ///< No pending update — device is already up to date
  OTA_SUCCESS,        ///< Firmware flashed OK; ESP.restart() has been called
  OTA_FAILED,         ///< Download or flash failed; status reported to server
  OTA_ERROR           ///< Network / API error; could not reach the server
};

/**
 * @brief Check for a pending OTA update and apply it.
 *
 * WiFi must already be connected before calling this function.
 * On success the ESP32 restarts automatically — the function never returns
 * OTA_SUCCESS to the caller.
 *
 * @param authToken   Device auth token (from Org Portal → Devices).
 * @param apiServer   Backend base URL, no trailing slash.
 *                    e.g. "http://192.168.1.10:8000" or "https://api.example.com"
 * @return OTAResult
 */
OTAResult checkAndApplyOTA(const char* authToken, const char* apiServer);

#endif // TL_OTA_H
