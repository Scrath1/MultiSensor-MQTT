#include "webserver_helpers.h"
#include "cfg.h"
#include "global_objects.h"
#include "filesystem/Filesystem.h"

bool isNumeric(const AsyncWebParameter* const p){
    String str = p->value();
    for(byte i=0; i<str.length();i++){
        if(!isDigit(str[i]) && !(i==0 && str[i]=='-')){
            // string may not contain non-numeric characters,
            // unless it is the first character and a minus sign
            return false;
        }
    }
    return true;
}

bool paramToInt(const AsyncWebParameter* const p, int32_t& output){
    if(isNumeric(p)){
        String str = p->value();
        output = str.toInt();
        return true;
    }
    else return false;
}

bool getLogMessagesFromTo(AsyncResponseStream* response, int32_t from, int32_t to){
    // Either two negative or two positive indices are allowed, but not mixed
    if((from < 0) != (to < 0)) return false;

    // make sure that from has the smaller absolute value
    if(std::abs(from) > std::abs(to)){
        int32_t tmp = from;
        from = to;
        to = tmp;
    }

    // Range check. If it exceeds the range, limit the to index to the maximum
    const uint32_t av = ramLogger.available();
    if(from < 0){
        // if negative indexing is used, to may at most be the opposite of
        // the number of available messages
        if(to < static_cast<int32_t>(av)*-1) to = static_cast<int32_t>(av)*-1;
    }
    else{
        if(to >= av) to = av;
    }
    
    DynamicJsonDocument doc(DYNAMIC_JSON_DOCUMENT_SIZE);
    JsonArray array = doc.to<JsonArray>();
    // Get individual log messages
    for(uint32_t i = std::abs(from); i <= std::abs(to); i++){
        char str[RAMLOGGER_MAX_STRING_LENGTH+1];
        uint32_t msgNumber = 0;
        if(from < 0){
            ramLogger.get(static_cast<int32_t>(i)*-1, str, RAMLOGGER_MAX_STRING_LENGTH, msgNumber);
        }
        else{
            ramLogger.get(static_cast<int32_t>(i), str, RAMLOGGER_MAX_STRING_LENGTH, msgNumber);
        }
        
        // Create JSON object for each message
        JsonObject obj = array.createNestedObject();
        obj["msg"] = str;
        obj["id"] = msgNumber;
    }
    serializeJson(array, *response);
    return true;
}

bool getFromToIndices(const AsyncWebServerRequest* const request, int32_t& fromIdx, int32_t& toIdx){
    if(request->hasParam("from") && request->hasParam("to")){
        AsyncWebParameter* from = request->getParam("from");
        AsyncWebParameter* to = request->getParam("to");

        if(!paramToInt(from, fromIdx) && fromIdx >= 0) return false;
        if(!paramToInt(to, toIdx) && toIdx >= 0) return false;

        return true;
    }
    else return false;
}

void getSystemInfo(AsyncResponseStream* response){
    DynamicJsonDocument doc(DYNAMIC_JSON_DOCUMENT_SIZE);
    JsonObject root = doc.to<JsonObject>();

    // WiFi
    JsonObject wifiObj = root.createNestedObject("WiFi");
    char modeNodeName[] = "Mode";
    switch(WiFi.getMode()){
        default:
            wifiObj[modeNodeName] = "NULL";
            break;
        case WIFI_MODE_STA:
            wifiObj[modeNodeName] = "Station";
            break;
        case WIFI_MODE_AP:
            wifiObj[modeNodeName] = "soft-AP";
            break;
        case WIFI_MODE_APSTA:
            wifiObj[modeNodeName] = "Station + Soft-AP";
            break;
    }
    wifiObj["IP"] = WiFi.localIP().toString();
    wifiObj["TX Power"] = WiFi.getTxPower();
    wifiObj["RSSI"] = WiFi.RSSI();

    // serialize json
    serializeJson(doc, *response);
}

void getCurrentSensorData(JsonObject& obj){
    for(Sensor* s:sensors){
        obj[s->getName()] = s->readSensor();
    }
}

void getConfig(AsyncResponseStream* response){
    DynamicJsonDocument doc(DYNAMIC_JSON_DOCUMENT_SIZE);
    JsonObject root = doc.to<JsonObject>();

    root["ssid"] = settings.wifi.ssid;
    root["brokerAddress"] = settings.mqtt.brokerAddress;
    root["brokerPort"] = settings.mqtt.brokerPort;
    root["username"] = settings.mqtt.username;
    root["deviceTopic"] = settings.mqtt.deviceTopic;
    root["clientID"] = settings.mqtt.clientID;

    // serialize json
    serializeJson(doc, *response);
}