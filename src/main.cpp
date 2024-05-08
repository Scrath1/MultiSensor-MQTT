#include <Arduino.h>

#include <PubSubClient.h>
#include <WiFi.h>
#include <vector>

#include "webserver/webserver.h"
#include "global_objects.h"
#include "helper_functions.h"
#include "sensors/SensorFactory.h"

WiFiClient espWiFiClient;
PubSubClient mqttClient(espWiFiClient);

const char *encryptionTypeToString(wifi_auth_mode_t encryptionType) {
    switch (encryptionType) {
        case WIFI_AUTH_OPEN:
            return "open";
        case WIFI_AUTH_WEP:
            return "WEP";
        case WIFI_AUTH_WPA_PSK:
            return "WPA";
        case WIFI_AUTH_WPA2_PSK:
            return "WPA2";
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WPA+WPA2";
        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "WPA2-EAP";
        case WIFI_AUTH_WPA3_PSK:
            return "WPA3";
        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WPA2+WPA3";
        case WIFI_AUTH_WAPI_PSK:
            return "WAPI";
        default:
            return "unknown";
    }
}

/**
 * @brief Scans nearby networks and prints them to the logs
 *
 * @return uint32_t Number of networks found
 */
uint32_t wifiScan() {
    const uint32_t n = WiFi.scanNetworks();
    if (0 == n) {
        ramLogger.logLn("No networks found");
    } else {
        ramLogger.logLnf("%2u networks found. Listing up to 5 strongest", n);
        ramLogger.logLn("Nr | SSID                             | RSSI | CH | Encryption");
        for (uint32_t i = 0; i < n && i < 5; i++) {
            ramLogger.logLnf("%2u | %-32.32s | %4d | %2d | %s", i + 1, WiFi.SSID(i), WiFi.RSSI(i),
                             WiFi.channel(i), encryptionTypeToString(WiFi.encryptionType(i)));
        }
    }
    WiFi.scanDelete();
    return n;
}

void wifiSetup() {
    ramLogger.logLn("Setting up WiFi");
    // Useful for debugging but can cause issues when an SSID has non ASCII that
    // get misinterpreted by the JSON debugger and therefore prevent the printing of logs
    // in the webinterface
    wifiScan();

    WiFi.softAP(WIFI_AP_NAME);
    WiFi.mode(WIFI_STA);
    WiFi.begin(settings.wifi.ssid, settings.wifi.password);
    if (WL_CONNECTED != WiFi.waitForConnectResult(WIFI_CONNECTION_TIMEOUT_MS)) {
        // WiFi setup failed
        ramLogger.logLnf("Failed to connect to %s network. Starting SoftAP instead",
            settings.wifi.ssid);
        WiFi.softAP(WIFI_AP_NAME);
    } else {
        // successfull connection
        ramLogger.logLnf("Successful connection. IP Address: %s", WiFi.localIP().toString().c_str());
        WiFi.setAutoReconnect(true);
    }
}

/**
 * @brief Parses the given sensor config file and creates the
 * contained sensors and their pipeline stages
 * 
 * @param filename [IN]
 * @return RC_t RC_SUCCESS on success
 */
RC_t parseSensorFile(const char filename[]) {
    // Try to open the given file
    RC_t err = filesystem->openFile(filename, Filesystem::READ_ONLY);
    if (err != RC_SUCCESS){
        ramLogger.logLnf("Failed to open %s", SENSOR_CFG_FILENAME);
        return err;
    }

    while (true){
        uint8_t data[1024] = "\0";
        // Read until [
        err = filesystem->readUntil(data, sizeof(data), SENSOR_CFG_OPEN_CHAR[0]);
        if (err != RC_SUCCESS) break;

        // Trim all comments and leading whitespace from the input
        trimComments(reinterpret_cast<char *>(data), CONFIG_FILE_COMMENT_DELIMITER);
        trimLeadingWhitespace(reinterpret_cast<char *>(data));

        // if the string is now empty, stop parsing
        uint32_t dataStrLen = strlen(reinterpret_cast<char *>(data));
        if(dataStrLen == 0) break;

        char* commentStart = strstr(reinterpret_cast<char *>(data), CONFIG_FILE_COMMENT_DELIMITER);
        // if a comment delimiter was found, the data read is part of an open ended comment.
        if(commentStart != nullptr){
            uint32_t matchedCharsInRow = 0;
            while(matchedCharsInRow != strlen(CONFIG_FILE_COMMENT_DELIMITER)){
                char tmp[2] = "\0";
                filesystem->read(reinterpret_cast<uint8_t *>(tmp), 1);
                // count up when the char order matches that of the comment delimiter.
                if(tmp[0] == CONFIG_FILE_COMMENT_DELIMITER[matchedCharsInRow])
                    matchedCharsInRow++;
                else // reset counter to 0 when a mismatched char is found
                    matchedCharsInRow = 0;
            }
            // after rest of open comment was removed, skip to next read operation
            continue;
        }

        // Now only the type of the sensor should remain.
        // Copy it to a separate string and parse the config string for that sensor
        char sensorTypeStr[128];
        strcpy(sensorTypeStr, reinterpret_cast<const char *>(data));
        ramLogger.logLnf("Found %s", sensorTypeStr);

        // Read configuration of the found sensor and its pipeline stages
        err = filesystem->readUntil(data, sizeof(data), SENSOR_CFG_CLOSE_CHAR[0]);
        if (err != RC_SUCCESS) break;
        trimComments(reinterpret_cast<char *>(data), CONFIG_FILE_COMMENT_DELIMITER);
        trimLeadingWhitespace(reinterpret_cast<char *>(data));

        // Create sensor
        Sensor *ptr = SensorFactory::sensorFromConfigString(sensorTypeStr, reinterpret_cast<char *>(data));
        if(ptr == nullptr){
            ramLogger.logLnf("Failed to create %s", sensorTypeStr);
            break;
        }
        else{
            ramLogger.logLnf("Created sensor %s with %u pipeline stages",
                ptr->getName(), ptr->getNumPipelineStages());
            sensors.push_back(ptr);
        }
    }
    // Whatever happened, close the file
    filesystem->closeFile();
    // Return error code if one occurred
    return err;
}

#ifndef PIO_UNIT_TESTING

void setup() {
    Serial.begin(115200);
    // Wait after flashing/boot to allow terminal to connect
    sys_delay_ms(2000);
    // put your setup code here, to run once:
    ramLogger.logLn("MultiSensor-MQTT");
    if(filesystem->isInitialized())
        ramLogger.logLn("Successfully mounted filesystem");
    else{
        ramLogger.logLn("Failed to mount filesystem");
        while(1);
    }

    // Read settings
    if(RC_SUCCESS != parseSettingsFile(CONFIG_FILENAME, settings))
        ramLogger.logLn("Failed to parse config file");
    else
        ramLogger.logLn("Successfully parsed config file");

    // if there was no clientID specified, generate one
    if(strlen(settings.mqtt.clientID) == 0)
        snprintf(settings.mqtt.clientID, 48, "MultiSensor-MQTT-%llX", ESP.getEfuseMac());
    if(strlen(settings.mqtt.deviceTopic) == 0)
        snprintf(settings.mqtt.clientID, 16, "%llX", ESP.getEfuseMac());

    // Preferences initialization
    preferences.begin("MultiSensor", false);
    if(!preferences.isKey("WIFI_Password"))
        preferences.putString("WIFI_Password", "PlaceholderPassword");
    if(!preferences.isKey("MQTT_Password"))
        preferences.putString("MQTT_Password", "PlaceholderPassword");
    // Read passwords from preferences
    preferences.getString("WIFI_Password", settings.wifi.password,
        sizeof(settings.wifi.password));
    preferences.getString("MQTT_Password", settings.mqtt.password,
        sizeof(settings.mqtt.password));

    wifiSetup();
    webserverSetup();

    // mqtt setup
    IPAddress serverIP;
    serverIP.fromString(settings.mqtt.brokerAddress);
    ramLogger.logLnf("MQTT broker address: %s", serverIP.toString().c_str());
    mqttClient.setServer(
        serverIP,
        settings.mqtt.brokerPort);

    // Sensor setup
    if(RC_SUCCESS != parseSensorFile(SENSOR_CFG_FILENAME)){
        ramLogger.logLn("Failed to parse sensor file");
    }
    else{
        ramLogger.logLn("Successfully parsed sensor config file");
    }
}

void reconnect() {
    // Loop until we're reconnected
    while (!mqttClient.connected()) {
        ramLogger.logLnf("Attempting MQTT connection, ID: %s, User: %s",
            settings.mqtt.clientID, settings.mqtt.username);
        // Attempt to connect
        if (mqttClient.connect(
                settings.mqtt.clientID,
                settings.mqtt.username,
                settings.mqtt.password)) {
            Serial.println("connected");
        } else {
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void loop() {
    static uint32_t delayCounter = 0;
    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop();
    delay(1000);
    if (delayCounter % SENSOR_POLLING_INTERVAL_S == 0) {
        delayCounter = 0;
        // publish value
        for(Sensor* s : sensors){
            if(s == nullptr) continue;
            float_t val = s->readSensor();
            Serial.print(s->getName());
            Serial.print(": ");
            Serial.println(val);
            // If the client is connected, publish the sensor data
            if(mqttClient.connected()){
                char topic[256] = "";
                snprintf(topic, sizeof(topic), "%s/%s/%s",
                    MQTT_BASE_TOPIC, settings.mqtt.deviceTopic,
                    s->getName());
                char valStr[32];
                snprintf(valStr, sizeof(valStr), "%f", val);
                mqttClient.publish(topic, valStr);
            }
        }
    }
    delayCounter++;
}

#endif  // PIO_UNIT_TESTING