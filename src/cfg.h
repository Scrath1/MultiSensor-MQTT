#ifndef CFG_H
#define CFG_H

// WiFi configurations
// ============================================

// The name of the WiFi Access point the ESP will open if it cannot connect to an existing station
#define WIFI_AP_NAME "ESP32"
// Time in ms after which the ESP will give up trying to connect to an existing station
// and open its own wifi access point
#define WIFI_CONNECTION_TIMEOUT_MS (30000)

// RamLogger configurations
// ============================================

// Configures the maximum length of a log string which can be saved
// in the ram logger for later access. Does not include the null-terminator.
// Larger strings will still print to serial but will be cut off in the buffer
#define RAMLOGGER_MAX_STRING_LENGTH (100)
// Defines the amount of messages which are buffered in the
// ram logger before the oldest one is overwritten.
#define RAMLOGGER_MAX_MESSAGE_COUNT (50)

#endif // CFG_H