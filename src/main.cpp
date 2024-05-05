#include <Arduino.h>

#if __has_include("wifi_credentials.h")
/**
 * This is an include for a header file which is not tracked in git.
 * It contains a define WIFI_SSID and WIFI_PASSWORD for connecting to
 * a predefined WiFi AP while debugging. If the file does not exist,
 * the program should still be able to build but will directly start
 * a SoftAP rather than trying to connect to an existing WiFi network
 * first.
 */
#include "wifi_credentials.h"
#endif

#include <PubSubClient.h>
#include <WiFi.h>

#include <vector>

#include "global_objects.h"
#include "helper_functions.h"
#include "sensors/SensorFactory.h"

std::vector<Sensor*> sensors;

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
#ifndef WIFI_SSID
    ramLogger.logLn("Starting SoftAP");
    WiFi.softAP(WIFI_AP_NAME);
#else
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    if (WL_CONNECTED != WiFi.waitForConnectResult(WIFI_CONNECTION_TIMEOUT_MS)) {
        // WiFi setup failed
        ramLogger.logLnf("Failed to connect to %s network. Starting SoftAP instead", WIFI_SSID);
        WiFi.softAP(WIFI_AP_NAME);
    } else {
        // successfull connection
        ramLogger.logLnf("Successful connection. IP Address: %s", WiFi.localIP().toString().c_str());
        WiFi.setAutoReconnect(true);
    }
#endif  // WIFI_SSID
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
    if (err != RC_SUCCESS) return err;

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
    if(RC_SUCCESS != err) return err;
    else return RC_SUCCESS;
}

#ifndef PIO_UNIT_TESTING

IPAddress serverIp(192, 168, 1, 103);
WiFiClient espWiFiClient;
PubSubClient mqttClient(espWiFiClient);

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
    // wifiSetup();

    // mqtt setup
    // mqttClient.setServer(serverIp, 1883);

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
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect("arduinoClient")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void loop() {
    static uint32_t delayCounter = 0;
    // if (!mqttClient.connected()) {
    //     reconnect();
    // }
    // mqttClient.loop();
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
        }
    }
    delayCounter++;
}

#endif  // PIO_UNIT_TESTING