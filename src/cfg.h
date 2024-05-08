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

// Sensor config file
// ============================================

// Name of the sensor config file. Keep in mind that LittleFS requires
// absolute file paths.
#define SENSOR_CFG_FILENAME "/sensor_config.txt"
// Character with which the beginning of sensor config information is marked
// in the sensor config file
#define SENSOR_CFG_OPEN_CHAR "["
// Character with which the end of sensor config information is marked
// in the sensor config file
#define SENSOR_CFG_CLOSE_CHAR "]"
// Character with which the beginning of transformer config information is marked
// in the sensor config file
#define TRANSFORMER_CFG_OPEN_CHAR "{"
// Character with which the end of transformer config information is marked
// in the sensor config file
#define TRANSFORMER_CFG_CLOSE_CHAR "}"
// Characters with which the beginning and end of comments in the config files are marked
#define CONFIG_FILE_COMMENT_DELIMITER "//"

// General
// ============================================

// Defines (in seconds) how often the sensor is read and its
// result printed to serial
#define SENSOR_POLLING_INTERVAL_S 10

// Top-level topic for this sensor platform
#define MQTT_BASE_TOPIC "MultiSensor-MQTT"

// Name of the general configuration file. Keep in mind that
// LittleFS requires absolute file paths.
#define CONFIG_FILENAME "/config.txt"

// Webserver configuration
// ============================================

// The maximum size of a dynamically allocated JSON response string.
// This limits, how many sensor values can be returned at once over the REST API
#define DYNAMIC_JSON_DOCUMENT_SIZE (2048)
// Enables UART logging calls for when a request was received or answered by the webserver.
// Disabled when not debugging for better performance
#define ENABLE_WEBSERVER_REQUEST_LOGGING (0)

#endif // CFG_H