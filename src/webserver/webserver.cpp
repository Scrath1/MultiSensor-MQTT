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

        // ToDo: Extend for settings form
        uint32_t changedSettingCount = 0;
        if(request->hasParam("WifiSSID",true)){
            changedSettingCount++;
        }
        
        if(request->hasParam("restart", true)){
            // Restart system
            request->send(200);
            ramLogger.logLn("Restarting system in 5 seconds");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            ESP.restart();
        }
        else if(request->args() == 0){
            ramLogger.logLn("Received /api/system POST request without parameters");
            request->send(400, "text/plain", "POST request without parameters");
        }
        else{
            ramLogger.logLn("Invalid POST request parameters");
            request->send(400, "text/plain", "Invalid POST request parameters");
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