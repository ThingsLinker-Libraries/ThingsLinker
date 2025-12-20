/**
 * @file TL_Config.h
 * @brief Configuration file for ThingsLinker
 *
 * All settings in one place - easy to modify!
 */

#ifndef TL_CONFIG_H
#define TL_CONFIG_H

// ========== Virtual Pins Configuration ==========
// Supports V0 to V124 (125 pins total)
#define MAX_VIRTUAL_PINS 125

// ========== MQTT Server Configuration ==========
// Production MQTT server (uncomment for production use)
// #define MQTT_SERVER "mqtt.thingslinker.com"
// #define MQTT_PORT 1883

// ========== Test MQTT Server (HiveMQ Public Broker) ==========
// For testing without ThingsLinker backend (CURRENTLY ACTIVE)
#define MQTT_SERVER "mqtt.thingslinker.com"
#define MQTT_PORT 1883
// Note: HiveMQ is public - anyone can see your data. Use only for testing!

// ========== BLE Configuration ==========
#define BLE_DEVICE_NAME_PREFIX "ThingsLinker_"
#define BLE_SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_WIFI_CHAR_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define BLE_STATUS_CHAR_UUID "cba1d466-344c-4be3-ab3f-189f80dd7518"
#define BLE_CONFIRM_CHAR_UUID "8ec90774-f8a8-4f5c-8e5c-3f9a7d8c6b2a"

// ========== WiFi Configuration ==========
#define WIFI_CONNECT_TIMEOUT 30000  // 30 seconds
#define WIFI_RETRY_INTERVAL 5000    // 5 seconds

// ========== MQTT Packet Size ==========
// CRITICAL: Must be large enough for LWT messages with long credentials
// Default PubSubClient is 256 bytes - NOT ENOUGH for ThingsLinker credentials
// Required size: ~200+ bytes for LWT packet with full credentials
#define MQTT_MAX_PACKET_SIZE 512  // Increased from default 256

// ========== Timeouts ==========
#define MQTT_KEEPALIVE 60
#define STATUS_UPDATE_INTERVAL 60000  // Send status every 60 seconds

// ========== Debug ==========
#define DEBUG_ENABLED true  // Set to false to disable all debug messages

#endif // TL_CONFIG_H
