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

#include <WiFi.h>
#include "global_objects.h"
#include <LittleFS.h>

const char *encryptionTypeToString(wifi_auth_mode_t encryptionType)
{
    switch (encryptionType)
    {
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
uint32_t wifiScan(){
    const uint32_t n = WiFi.scanNetworks();
    if(0 == n){
        ramLogger.logLn("No networks found");
    }
    else{
        ramLogger.logLnf("%2u networks found. Listing up to 5 strongest", n);
        ramLogger.logLn("Nr | SSID                             | RSSI | CH | Encryption");
        for(uint32_t i = 0; i < n && i < 5; i++){
            ramLogger.logLnf("%2u | %-32.32s | %4d | %2d | %s", i+1, WiFi.SSID(i), WiFi.RSSI(i),
                WiFi.channel(i), encryptionTypeToString(WiFi.encryptionType(i)));
        }
    }
    WiFi.scanDelete();
    return n;
}

void wifiSetup(){
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
        if(WL_CONNECTED != WiFi.waitForConnectResult(WIFI_CONNECTION_TIMEOUT_MS)){
            // WiFi setup failed
            ramLogger.logLnf("Failed to connect to %s network. Starting SoftAP instead", WIFI_SSID);
            WiFi.softAP(WIFI_AP_NAME);
        }
        else{
            // successfull connection
            ramLogger.logLnf("Successful connection. IP Address: %s",  WiFi.localIP().toString().c_str());
            WiFi.setAutoReconnect(true);
        }
    #endif // WIFI_SSID
}

#ifndef PIO_UNIT_TESTING

void setup() {
    Serial.begin(115200);
    // Wait after flashing/boot to allow terminal to connect
    sys_delay_ms(2000);
    // put your setup code here, to run once:
    ramLogger.logLn("MultiSensor-MQTT");
    wifiSetup();
}

void loop() {
    RUN_ONCE{
        Serial.print("Loop task core id: ");
        Serial.println(xPortGetCoreID());
    }
}

#endif // PIO_UNIT_TESTING