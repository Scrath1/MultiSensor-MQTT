#include "settings.h"

#include "global_objects.h"
#include "helper_functions.h"

RC_t parseSettingsFile(const char filename[], settings_t &settingsObject) {
    // Try to open the given file
    RC_t err = filesystem->openFile(filename, Filesystem::READ_ONLY);
    if (err != RC_SUCCESS) return err;

    // Flag to indicate whether the WiFi password
    // was found in the settings file.
    // If it was, rewrite the file without it
    bool passwordFound = false;

    while (!filesystem->eof()) {
        uint8_t data[1024] = "\0";
        // Read line
        err = filesystem->read(data, sizeof(data));
        if (err != RC_SUCCESS) break;

        // Remove comments from input
        trimComments(reinterpret_cast<char *>(data), CONFIG_FILE_COMMENT_DELIMITER);

        // Parse the key-value pairs required for the settings
        char value[128] = "";
        RC_t parseErr = readKeyValue(reinterpret_cast<char *>(data),
                                     "WIFI_SSID", value, sizeof(value));
        if (parseErr == RC_SUCCESS) strcpy(settingsObject.wifi.ssid, value);
        memset(value, '\0', sizeof(value));

        parseErr = readKeyValue(reinterpret_cast<char *>(data),
                                "WIFI_Password", value, sizeof(value));
        if (parseErr == RC_SUCCESS) {
            ramLogger.logLn("Found WiFi password in config file. Removing it...");
            strcpy(settingsObject.wifi.password, value);
            passwordFound = true;
        }
        memset(value, '\0', sizeof(value));

        parseErr = readKeyValue(reinterpret_cast<char *>(data),
                                "MQTT_Broker_Address", value, sizeof(value));
        if (parseErr == RC_SUCCESS) strcpy(settingsObject.mqtt.brokerAddress, value);
        memset(value, '\0', sizeof(value));

        parseErr = readKeyValueInt(reinterpret_cast<char *>(data),
                                   "MQTT_Broker_Port", reinterpret_cast<int32_t &>(settingsObject.mqtt.brokerPort));
        if (RC_ERROR_INVALID == parseErr)
            ramLogger.logLn("Config: Could not parse MQTT broker port");

        parseErr = readKeyValue(reinterpret_cast<char *>(data),
                                "MQTT_ClientID", value, sizeof(value));
        if (parseErr == RC_SUCCESS) strcpy(settingsObject.mqtt.clientID, value);
        memset(value, '\0', sizeof(value));

        parseErr = readKeyValue(reinterpret_cast<char *>(data),
                                "MQTT_Username", value, sizeof(value));
        if (parseErr == RC_SUCCESS) strcpy(settingsObject.mqtt.username, value);
        memset(value, '\0', sizeof(value));

        parseErr = readKeyValue(reinterpret_cast<char *>(data),
                                "MQTT_Device_Topic", value, sizeof(value));
        if (parseErr == RC_SUCCESS) strcpy(settingsObject.mqtt.deviceTopic, value);
        memset(value, '\0', sizeof(value));
    }
    filesystem->closeFile();

    // Remove password from settings file
    if (passwordFound) writeToSettingsFile(filename, settingsObject);
    return err;
}

RC_t writeToSettingsFile(const char filename[], const settings_t &settingsObject) {
    // Try to open the given file in Truncate mode to overwrite it
    RC_t err = filesystem->openFile(filename, Filesystem::WRITE_TRUNCATE);
    if (err != RC_SUCCESS) return err;

    char line[512] = "";
    int32_t usedChars = snprintf(line, sizeof(line), "WIFI_SSID: %s\n",
                                 settingsObject.wifi.ssid);
    if (RC_SUCCESS != filesystem->write((uint8_t *)line, usedChars))
        return RC_ERROR_WRITE_FAILS;
    usedChars = snprintf(line, sizeof(line), "MQTT_Broker_Address: %s\n",
                         settingsObject.mqtt.brokerAddress);
    if (RC_SUCCESS != filesystem->write((uint8_t *)line, usedChars))
        return RC_ERROR_WRITE_FAILS;
    usedChars = snprintf(line, sizeof(line), "MQTT_Broker_Port: %u\n",
                         settingsObject.mqtt.brokerPort);
    if (RC_SUCCESS != filesystem->write((uint8_t *)line, usedChars))
        return RC_ERROR_WRITE_FAILS;
    usedChars = snprintf(line, sizeof(line), "MQTT_ClientID: %s\n",
                         settingsObject.mqtt.clientID);
    if (RC_SUCCESS != filesystem->write((uint8_t *)line, usedChars))
        return RC_ERROR_WRITE_FAILS;
    usedChars = snprintf(line, sizeof(line), "MQTT_Username: %s\n",
                         settingsObject.mqtt.username);
    if (RC_SUCCESS != filesystem->write((uint8_t *)line, usedChars))
        return RC_ERROR_WRITE_FAILS;
    usedChars = snprintf(line, sizeof(line), "MQTT_Device_Topic: %s\n",
                         settingsObject.mqtt.deviceTopic);
    if (RC_SUCCESS != filesystem->write((uint8_t *)line, usedChars))
        return RC_ERROR_WRITE_FAILS;

    filesystem->closeFile();
    return RC_SUCCESS;
}