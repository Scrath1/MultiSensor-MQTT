#include "webserver.h"

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "cfg.h"
#include "webserver_helpers.h"
#include "global_objects.h"
#include "filesystem/Filesystem.h"

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void sensorEndpointSetup(){
    /**
     * If no get parameters are passed, give last sensor value.
     * If a from and to parameter are passed, return a range of values.
     * If sampleCount is passed as a parameter, return the total number of available samples
    */
    server.on("/api/sensors", HTTP_GET,[](AsyncWebServerRequest *request){
        #if ENABLE_WEBSERVER_REQUEST_LOGGING
        Serial.println("Webserver: Sensor data request received");
        #endif // ENABLE_WEBSERVER_REQUEST_LOGGING
        DynamicJsonDocument doc(DYNAMIC_JSON_DOCUMENT_SIZE);
        JsonObject obj = doc.to<JsonObject>();

        // Fill JSON object with sensor data
        getCurrentSensorData(obj);

        // Serialize JSON response and send it
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    });
}

void logEndpointSetup(){
    server.on("/api/log",HTTP_GET,[](AsyncWebServerRequest *request){
        #if ENABLE_WEBSERVER_REQUEST_LOGGING
        Serial.println("Webserver: Log request received");
        #endif // ENABLE_WEBSERVER_REQUEST_LOGGING
        if(request->args() == 0){
            // no params, just return last log message
            AsyncResponseStream* response = request->beginResponseStream("application/json");
            getLogMessagesFromTo(response, -1, -1);
            request->send(response);
        }
        else if(request->hasParam("from") && request->hasParam("to")){
            // range based data request
            int32_t fromIdx, toIdx;
            if(!getFromToIndices(request, fromIdx, toIdx)) {
                // Couldn't get indices. Send an error code and return
                request->send(400);
                return;
            }

            AsyncResponseStream* response = request->beginResponseStream("application/json");
            if(getLogMessagesFromTo(response, fromIdx, toIdx)){
                // Success. Send log messages
                request->send(response);
            }
            else{
                // invalid indices.
                // HTTP 400 Bad Request
                request->send(400);
            }
        }
        #if ENABLE_WEBSERVER_REQUEST_LOGGING
        Serial.println("Webserver: Log request answered");
        #endif // ENABLE_WEBSERVER_REQUEST_LOGGING
    });
}

void systemEndpointSetup(){
    // System setting changes
    server.on("/api/system", HTTP_POST, [](AsyncWebServerRequest *request){
        #if ENABLE_WEBSERVER_REQUEST_LOGGING
        Serial.println("Received /api/system HTTP POST request");
        #endif // ENABLE_WEBSERVER_REQUEST_LOGGING
        // check for post parameter time

        // Parameter handling
        uint32_t changedSettingCount = 0;
        if(request->hasParam("ssid", true)){
            AsyncWebParameter* p = request->getParam("ssid", true);
            strcpy(settings.wifi.ssid, p->value().c_str());
            ramLogger.logLnf("Updated SSID to %s", p->value().c_str());
            changedSettingCount++;
        }
        if(request->hasParam("ssid", true)){
            AsyncWebParameter* p = request->getParam("ssid", true);
            strcpy(settings.wifi.ssid, p->value().c_str());
            ramLogger.logLnf("Updated SSID to %s", p->value().c_str());
            changedSettingCount++;
        }
        if(request->hasParam("wifiPassword", true)){
            AsyncWebParameter* p = request->getParam("wifiPassword", true);
            strcpy(settings.wifi.password, p->value().c_str());
            // write to non-volatile storage
            preferences.putString("WIFI_Password", p->value().c_str());
            ramLogger.logLn("Updated WiFi password");
            changedSettingCount++;
        }
        if(request->hasParam("brokerAddress", true)){
            AsyncWebParameter* p = request->getParam("brokerAddress", true);
            strcpy(settings.mqtt.brokerAddress, p->value().c_str());
            ramLogger.logLnf("Updated broker address to %s", p->value().c_str());
            changedSettingCount++;
        }
        if(request->hasParam("brokerPort", true)){
            AsyncWebParameter* p = request->getParam("brokerPort", true);
            
            int32_t port = 0;
            // make sure that the port parameter is numeric
            if(paramToInt(p, port)){
                settings.mqtt.brokerPort = port;
                ramLogger.logLnf("Updated broker port to %u", port);
                changedSettingCount++;
            }
        }
        if(request->hasParam("username", true)){
            AsyncWebParameter* p = request->getParam("username", true);
            strcpy(settings.mqtt.username, p->value().c_str());
            ramLogger.logLnf("Updated username to %s", p->value().c_str());
            changedSettingCount++;
        }
        if(request->hasParam("mqttPassword", true)){
            AsyncWebParameter* p = request->getParam("mqttPassword", true);
            strcpy(settings.mqtt.password, p->value().c_str());
            // write to non-volatile storage
            preferences.putString("MQTT_Password", p->value().c_str());
            ramLogger.logLn("Updated MQTT password");
            changedSettingCount++;
        }
        if(request->hasParam("clientID", true)){
            AsyncWebParameter* p = request->getParam("clientID", true);
            strcpy(settings.mqtt.clientID, p->value().c_str());
            ramLogger.logLnf("Updated clientID to %s", p->value().c_str());
            changedSettingCount++;
        }
        if(request->hasParam("deviceTopic", true)){
            AsyncWebParameter* p = request->getParam("deviceTopic", true);
            strcpy(settings.mqtt.deviceTopic, p->value().c_str());
            ramLogger.logLnf("Updated deviceTopic to %s", p->value().c_str());
            changedSettingCount++;
        }
        if(request->hasParam("sensorConfig", true)){
            AsyncWebParameter* p = request->getParam("sensorConfig", true);
            if(RC_SUCCESS == filesystem->openFile(SENSOR_CFG_FILENAME, Filesystem::WRITE_TRUNCATE)){
                const char* text = p->value().c_str();
                // listAllParams(request);
                const uint32_t textLen = strlen(text);
                filesystem->write((uint8_t*) text, textLen);
                filesystem->closeFile();
                ramLogger.logLnf("Updated sensor settings file (%u characters written)", textLen);
                changedSettingCount++;
            }
            else ramLogger.logLn("Failed to update sensor config file");
        }

        // If any settings were changed, write them back and reboot
        if(changedSettingCount > 0){
            RC_t err = writeToSettingsFile(CONFIG_FILENAME, settings);
            if(RC_SUCCESS != err)
                ramLogger.logLnf("Failed to write settings file, Error Code=%i", err);
            else{
                // Short delay to finish printing potential debug messages
                ramLogger.logLn("Restarting system in 3 seconds");
                delay(3000);
                ESP.restart();
            }
        }

        // Check this parameter last as it will stop code execution after it
        if(request->hasParam("restart", true)){
            // Restart system
            request->send(200);
            ramLogger.logLn("Restarting system in 5 seconds");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            ESP.restart();
        }

        // Reply
        if(request->args() == 0){ // no arguments were received
            ramLogger.logLn("Received /api/system POST request without parameters");
            request->send(400, "text/plain", "POST request without parameters");
        }
        else if(changedSettingCount == 0){ 
            // some arguments were received but no settings updated
            // due to wrong parameters
            ramLogger.logLn("Invalid POST request parameters");
            request->send(400, "text/plain", "Invalid POST request parameters");
        }
        else{
            request->send(200);
        }
    });

    // General information requests
    server.on("/api/system", HTTP_GET, [](AsyncWebServerRequest *request){
        if(request->args() == 0){
            // No arguments, return general system information
            AsyncResponseStream* response = request->beginResponseStream("application/json");
            getSystemInfo(response);
            request->send(response);
        }
        else if(request->hasParam("getConfig")){
            AsyncResponseStream* response = request->beginResponseStream("application/json");
            getConfig(response);
            request->send(response);
        }
    });
}

void fileEndpointSetup(){
    // General information requests
    server.on("/api/file", HTTP_GET, [](AsyncWebServerRequest *request){
        if(request->hasParam("filename")){
            AsyncWebParameter* fnameParam = request->getParam("filename");
            
            if(filesystem->fileExists(fnameParam->value().c_str())){
                request->send(LittleFS, fnameParam->value().c_str());
            }
            else{
                request->send(404, "text/plain", "Not found");
            }
        }
        else if(request->hasParam("sensorConfig")){
            // on data param, always return the persisted data file if it exists
            if(filesystem->fileExists(SENSOR_CFG_FILENAME)){
                request->send(LittleFS, SENSOR_CFG_FILENAME);
            }
            else{
                ramLogger.logLn("Download request: Sensor config file does not exist");
            }
        }
    });
}

void webserverSetup(){
    // Required for XMLHttpRequests
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type");

    // Setup of endpoints
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        #if ENABLE_WEBSERVER_REQUEST_LOGGING
        Serial.println("Webserver: Client is connecting");
        #endif // ENABLE_WEBSERVER_REQUEST_LOGGING
        AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html", "text/html");
        request->send(response);
    });
    server.on("/system.html", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/system.html", "text/html");
        request->send(response);
    });
    server.on("/log.html", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/log.html", "text/html");
        request->send(response);
    });
    // Required to serve the stylesheet together with the website
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/style.css", "text/css");
    });
    // and for the javascript file
    server.on("/scripts.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/scripts.js", "application/javascript");
    });
    sensorEndpointSetup();
    logEndpointSetup();
    systemEndpointSetup();
    fileEndpointSetup();

    server.onNotFound(notFound);
    server.begin();
}