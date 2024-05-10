#ifndef SETTINGS_H
#define SETTINGS_H
#include "global.h"

typedef struct {
    struct {
        char ssid[64] = "";
        char password[64] = "";
        char hostname[64] = "";
    } wifi;
    struct {
        /**
         * @brief IPv4 address of the MQTT broker.
         * Set to 0.0.0.0 if not wanted.
         */
        char brokerAddress[16] = "0.0.0.0";
        uint32_t brokerPort = 0;
        /**
         * @brief Client ID must be unique
         */
        char clientID[64] = "";
        /**
         * @brief Optional username. Leave empty if not required.
         */
        char username[64] = "";
        /**
         * @brief Password to corresponding username
         */
        char password[64] = "";
        /**
         * @brief Topic by which multiple devices are differentiated
         * > MQTT_BASE_TOPIC/<deviceTopic>/<sensorName>
         */
        char deviceTopic[64] = "";
    } mqtt;
} settings_t;

/**
 * @brief Parses the given file into the provided settings object
 * as far as possible
 *
 * @param filename [IN] Name of the configuration file
 * @param settingsObject [OUT] Settings object to which the file contents are parsed
 * @return RC_t RC_SUCCESS on success,
 *  RC_ERROR_OPEN if the file could not be opened,
 *  RC_ERROR_BUSY if another file is currently open and needs to be closed first,
 *  RC_ERROR_READ_FAILS if the read operation failed
 */
RC_t parseSettingsFile(const char filename[], settings_t& settingsObject);

/**
 * @brief Overwrites the config file with the settings currently stored
 * in the provided settings object
 *
 * @param filename [IN] Name of the configuration file to overwrite
 * @param settingsObject [IN] Settings to write
 * @return RC_t RC_SUCCESS on success,
 *  RC_ERROR_OPEN if the file could not be opened,
 */
RC_t writeToSettingsFile(const char filename[], const settings_t& settingsObject);

#endif  // SETTINGS_H