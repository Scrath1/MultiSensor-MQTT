#include "mqtt.h"

#include <PubSubClient.h>
#include <WiFi.h>

#include "global.h"
#include "global_objects.h"

TaskHandle_t mqttTaskHandle;
WiFiClient espWiFiClient;
PubSubClient mqttClient(espWiFiClient);

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

void mqttTask(void* pvParameters) {
    TickType_t lastWakeTime;

    // setup
    mqttClient.setKeepAlive(MQTT_CONNECTION_KEEPALIVE_S);
    IPAddress serverIP;
    serverIP.fromString(settings.mqtt.brokerAddress);
    ramLogger.logLnf("MQTT broker address: %s", serverIP.toString().c_str());
    mqttClient.setServer(
        serverIP,
        settings.mqtt.brokerPort);

    while (1) {
        // Get current tick count for more precise cycle time calculation
        lastWakeTime = xTaskGetTickCount();

        // Process messages and maintain connection
        if (!mqttClient.connected()) {
            // lost connection. Try to reconnect...
            int8_t err = mqttClient.connect(
                settings.mqtt.clientID,
                settings.mqtt.username,
                settings.mqtt.password);
            bool stopFlag = false;
            switch (err) {
                default:
                    break;
                case MQTT_CONNECTION_TIMEOUT:
                    ramLogger.logLn("MQTT connection timeout");
                    break;
                case MQTT_CONNECTION_LOST:
                    ramLogger.logLn("MQTT connection lost");
                    break;
                case MQTT_CONNECT_FAILED:
                    ramLogger.logLn("MQTT connection failed");
                    break;
                case MQTT_CONNECT_BAD_PROTOCOL:
                    ramLogger.logLn("MQTT version not supported by server");
                    break;
                case MQTT_CONNECT_BAD_CLIENT_ID:
                    ramLogger.logLn("MQTT client ID rejected by server");
                    break;
                case MQTT_CONNECT_UNAVAILABLE:
                    ramLogger.logLn("MQTT server unavailable");
                    break;
                case MQTT_CONNECT_BAD_CREDENTIALS:
                    ramLogger.logLn("MQTT credentials rejected");
                    stopFlag = true;
                    break;
                case MQTT_CONNECT_UNAUTHORIZED:
                    ramLogger.logLn("MQTT client not authorized");
                    stopFlag = true;
                    break;
            }
            if (stopFlag) {
                ramLogger.logLn("Critical MQTT error. Halting MQTT task");
                vTaskSuspend(NULL);
            }
        }
        mqttClient.loop();

        // Sleep until next connection check is due
        if (pdFALSE == xTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(MQTT_TASK_CYCLE_TIME_MS))) {
            ramLogger.logLn("MQTT task cycle time too low");
        }
    }
    // Task should never return
}