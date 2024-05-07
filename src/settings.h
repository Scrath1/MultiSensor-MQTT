#ifndef SETTINGS_H
#define SETTINGS_H
#include "global.h"

typedef struct{
    struct{
        char ssid[64] = "";
        char password[64] = "";
    } wifi;
    struct{
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

#endif // SETTINGS_H