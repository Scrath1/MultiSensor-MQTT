#include <Arduino.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <freertos/task.h>

#include <vector>

#include "global_objects.h"
#include "helper_functions.h"
#include "mqtt.h"
#include "sensors/SensorFactory.h"
#include "webserver/webserver.h"

// Timer 0 used for automatic periodic reboot
#define TIMER0_PRESCALER (5120)
#define TIMER0_TICKS_PER_SECOND (APB_CLK_FREQ / TIMER0_PRESCALER)
#define TIMER0_TICKS (AUTO_REBOOT_INTERVAL_S * TIMER0_TICKS_PER_SECOND)
#if(TIMER0_TICKS > INT32_MAX)
    #error "Automatic reboot duration too long. Increase timer prescaler or decrease reboot time"
#endif
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
// Timer 0 used for automatic periodic reboot
hw_timer_t* timer0_Cfg = NULL;
bool rebootFlag = false;
static TaskHandle_t loopTaskHandle = NULL;

const char* encryptionTypeToString(wifi_auth_mode_t encryptionType) {
    switch(encryptionType) {
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
    if(0 == n) {
        Serial.println("No networks found");
    } else {
        Serial.printf("%2u networks found. Listing up to 5 strongest\n", n);
        Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
        for(uint32_t i = 0; i < n && i < 5; i++) {
            Serial.printf("%2u | %-32.32s | %4d | %2d | %s\n", i + 1, WiFi.SSID(i), WiFi.RSSI(i), WiFi.channel(i),
                          encryptionTypeToString(WiFi.encryptionType(i)));
        }
    }
    WiFi.scanDelete();
    return n;
}

void getTimestamp(char str[RAMLOGGER_MAX_TIMESTAMP_STR_LEN]) {
    if(!timeClient.update()) timeClient.forceUpdate();
    String formattedTime = timeClient.getFormattedTime();
    snprintf(str, RAMLOGGER_MAX_TIMESTAMP_STR_LEN, "%s UTC", formattedTime);
}

void ramLoggerPrintFunction(const char str[]) { Serial.print(str); }

void wifiSetup() {
    ramLogger.logLn("Setting up WiFi");
    wifiScan();

    WiFi.mode(WIFI_STA);
    WiFi.setHostname(settings.wifi.hostname);
    WiFi.begin(settings.wifi.ssid, settings.wifi.password);
    // WiFi.begin((const char*)settings.wifi.ssid, (const char*)settings.wifi.password);
    // Generate feedback in terminal every second
    for(uint32_t i = 0; i < WIFI_CONNECTION_TIMEOUT_MS / 1000; i++) {
        // if connected, break out of wait loop
        if(WL_CONNECTED == WiFi.status()) break;

        // To avoid spamming the RamLogger buffer (and because this is
        // just an idle message), this is printed directly to serial
        Serial.print('.');
        delay(1000);
    }
    Serial.print("\n");

    if(WL_CONNECTED != WiFi.status()) {
        // WiFi setup failed
        ramLogger.logLnf("Failed to connect to %s network. Status=%u Starting SoftAP instead", settings.wifi.ssid,
                         WiFi.status());
        WiFi.mode(WIFI_AP);
        if(!WiFi.softAP(WIFI_AP_NAME)) {
            ramLogger.logLn("Failed to setup softAP");
        }
    } else {
        // successfull connection
        ramLogger.logLnf(
            "Successful connection. IP Address: %s, "
            "DNS Server: %s",
            WiFi.localIP().toString().c_str(), WiFi.dnsIP().toString().c_str());
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
    if(err != RC_SUCCESS) {
        ramLogger.logLnf("Failed to open %s", SENSOR_CFG_FILENAME);
        return err;
    }

    while(true) {
        uint8_t data[1024] = "\0";
        // Read until [
        err = filesystem->readUntil(data, sizeof(data), SENSOR_CFG_OPEN_CHAR[0]);
        if(err != RC_SUCCESS) break;

        // Trim all comments and leading whitespace from the input
        trimComments(reinterpret_cast<char*>(data), CONFIG_FILE_COMMENT_DELIMITER);
        trimLeadingWhitespace(reinterpret_cast<char*>(data));

        // if the string is now empty, stop parsing
        uint32_t dataStrLen = strlen(reinterpret_cast<char*>(data));
        if(dataStrLen == 0) break;

        char* commentStart = strstr(reinterpret_cast<char*>(data), CONFIG_FILE_COMMENT_DELIMITER);
        // if a comment delimiter was found, the data read is part of an open ended comment.
        if(commentStart != nullptr) {
            uint32_t matchedCharsInRow = 0;
            while(matchedCharsInRow != strlen(CONFIG_FILE_COMMENT_DELIMITER)) {
                char tmp[2] = "\0";
                filesystem->read(reinterpret_cast<uint8_t*>(tmp), 1);
                // count up when the char order matches that of the comment delimiter.
                if(tmp[0] == CONFIG_FILE_COMMENT_DELIMITER[matchedCharsInRow])
                    matchedCharsInRow++;
                else  // reset counter to 0 when a mismatched char is found
                    matchedCharsInRow = 0;
            }
            // after rest of open comment was removed, skip to next read operation
            continue;
        }

        // Now only the type of the sensor should remain.
        // Copy it to a separate string and parse the config string for that sensor
        char sensorTypeStr[128];
        strcpy(sensorTypeStr, reinterpret_cast<const char*>(data));
        ramLogger.logLnf("Found %s", sensorTypeStr);

        // Read configuration of the found sensor and its pipeline stages
        err = filesystem->readUntil(data, sizeof(data), SENSOR_CFG_CLOSE_CHAR[0]);
        if(err != RC_SUCCESS) break;
        trimComments(reinterpret_cast<char*>(data), CONFIG_FILE_COMMENT_DELIMITER);
        trimLeadingWhitespace(reinterpret_cast<char*>(data));

        // Create sensor
        Sensor* ptr = SensorFactory::sensorFromConfigString(sensorTypeStr, reinterpret_cast<char*>(data));
        if(ptr == nullptr) {
            ramLogger.logLnf("Failed to create %s", sensorTypeStr);
            break;
        } else {
            ramLogger.logLnf("Created sensor %s with %u pipeline stages", ptr->getName(), ptr->getNumPipelineStages());
            sensors.push_back(ptr);
        }
    }
    // Whatever happened, close the file
    filesystem->closeFile();
    // Return error code if one occurred
    return err;
}

/**
 * @brief Used to trigger periodic automatic reboot using timer 0
 */
void IRAM_ATTR timer0_ISR() {
    rebootFlag = true;
    if(loopTaskHandle != NULL) {
        vTaskResume(loopTaskHandle);
    }
}

#ifndef PIO_UNIT_TESTING

void setup() {
    Serial.begin(115200);
    // Wait after flashing/boot to allow terminal to connect
    sys_delay_ms(2000);
    ramLogger.setPrintFunction(ramLoggerPrintFunction);
    // put your setup code here, to run once:
    ramLogger.logLn("MultiSensor-MQTT");
    if(filesystem->isInitialized())
        ramLogger.logLn("Successfully mounted filesystem");
    else {
        ramLogger.logLn("Failed to mount filesystem");
        while(1);
    }

    // Read settings
    if(RC_SUCCESS != parseSettingsFile(CONFIG_FILENAME, settings))
        ramLogger.logLn("Failed to parse config file");
    else
        ramLogger.logLn("Successfully parsed config file");

    // Preferences initialization
    // Passwords are not stored in the settings file.
    // If any are found during the first startup, they are moved to the
    // less easily readable Preference storage.
    preferences.begin("MultiSensor", false);

    // These initializations are only run once.
    // Unless the flash is fully erased, data stored in preferences can persist
    // through flashing of programs and filesystem images
    if(!preferences.isKey("WIFI_Password")) preferences.putString("WIFI_Password", "");
    if(!preferences.isKey("MQTT_Password")) preferences.putString("MQTT_Password", "");

    // Read passwords from preferences
    // If no password was found in the settings file, the password strings
    // should be empty at this point (meaning they have length 1 due to \0 byte)
    char tmp[sizeof(settings.wifi.password)] = "";
    preferences.getString("WIFI_Password", tmp, sizeof(tmp));
    if(strcmp(tmp, settings.wifi.password) != 0 && strlen(settings.wifi.password) > 1) {
        // string stored in preference storage does not match password found in config file
        // => Update value stored in flash
        preferences.putString("WIFI_Password", settings.wifi.password);
    } else {
        // No password initialization through config file.
        // => Copy password from preferences to wifi password variable
        strcpy(settings.wifi.password, tmp);
    }

    preferences.getString("MQTT_Password", tmp, sizeof(tmp));
    if(strcmp(tmp, settings.mqtt.password) != 0 && strlen(settings.mqtt.password) > 1) {
        // string stored in preference storage does not match password found in config file
        // => Update value stored in flash
        preferences.putString("MQTT_Password", settings.mqtt.password);
    } else {
        // No password initialization through config file.
        // => Copy password from preferences to mqtt password variable
        strcpy(settings.mqtt.password, tmp);
    }

    // if there was no clientID specified, generate one
    if(strlen(settings.mqtt.clientID) == 0)
        snprintf(settings.mqtt.clientID, 64, "MultiSensor-MQTT-%llX", ESP.getEfuseMac());
    // Same for the device topic
    if(strlen(settings.mqtt.deviceTopic) == 0) snprintf(settings.mqtt.deviceTopic, 64, "%llX", ESP.getEfuseMac());
    // And Hostname
    if(strlen(settings.wifi.hostname) == 0)
        snprintf(settings.wifi.hostname, 64, "MultiSensor-MQTT-%llX", ESP.getEfuseMac());

    wifiSetup();
    webserverSetup();

    // start timeclient
    timeClient.begin();
    ramLogger.setTimestampFunction(getTimestamp);

    // mqtt setup
    // If broker address is 0.0.0.0, don't use MQTT
    if(strcmp(settings.mqtt.brokerAddress, "0.0.0.0") != 0) {
        if(pdPASS !=
           xTaskCreate(mqttTask, MQTT_TASK_NAME, MQTT_TASK_STACK_SIZE, NULL, MQTT_TASK_PRIORITY, &mqttTaskHandle)) {
            ramLogger.logLn("Failed to create MQTT task");
        }
    }

    // Sensor setup
    if(RC_SUCCESS != parseSensorFile(SENSOR_CFG_FILENAME)) {
        ramLogger.logLn("Failed to parse sensor file");
    } else {
        ramLogger.logLn("Successfully parsed sensor config file");
    }

    // Auto reboot
    // ESP32C3 has 2 54-bit hardware timers with 16-bit pre-scalers
    timer0_Cfg = timerBegin(0, TIMER0_PRESCALER, true);
    timerAttachInterrupt(timer0_Cfg, timer0_ISR, true);
    timerAlarmWrite(timer0_Cfg, TIMER0_TICKS, false);
    ramLogger.logLnf("Started auto-reboot timer. Interval: %us", AUTO_REBOOT_INTERVAL_S);
    timerAlarmEnable(timer0_Cfg);
}

void loop() {
    if(loopTaskHandle == NULL) {
        loopTaskHandle = xTaskGetCurrentTaskHandle();
    }

    // stores the last waketime of the loop task
    static TickType_t lastWakeTime;
    lastWakeTime = xTaskGetTickCount();

    if(rebootFlag) {
        ramLogger.logLn("Automatic reboot triggered");
        delay(1000);
        ESP.restart();
    }

    // publish value
    for(Sensor* s : sensors) {
        if(s == nullptr) continue;
        float_t val = s->readSensor();
        float_t rawVal = s->readSensorRaw();
        Serial.print(s->getName());
        Serial.print(": ");
        Serial.print(val);
        Serial.print(", raw: ");
        Serial.println(rawVal);
        // If the mqtt client is connected, publish the sensor data
        if(mqttClient.connected()) {
            char topic[256] = "";
            char valStr[32] = "";

            // publish processed value
            snprintf(topic, sizeof(topic), "%s/%s/%s", MQTT_BASE_TOPIC, settings.mqtt.deviceTopic, s->getName());
            snprintf(valStr, sizeof(valStr), "%f", val);
            mqttClient.publish(topic, valStr);

            // publish raw value under subtopic
            snprintf(topic, sizeof(topic), "%s/%s/raw/%s", MQTT_BASE_TOPIC, settings.mqtt.deviceTopic, s->getName());
            snprintf(valStr, sizeof(valStr), "%f", rawVal);
            mqttClient.publish(topic, valStr);
        }
    }

    // Sleep task until the next sensor polling time
    xTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(SENSOR_POLLING_INTERVAL_S * 1000));
}

#endif  // PIO_UNIT_TESTING