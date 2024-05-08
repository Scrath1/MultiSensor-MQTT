#ifndef WEBSERVER_HELPERS_H
#define WEBSERVER_HELPERS_H
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "global.h"

/**
 * @brief Prints out all parameters in a request to serial
 * @param request [IN]
 */
void listAllParams(const AsyncWebServerRequest* request);

/**
 * @brief Checks if the given parameter string holds a numeric value
 *
 * @param p [IN] Parameter string to check
 * @return true Value is numeric
 * @return false Value is not numeric
 */
bool isNumeric(const AsyncWebParameter* const p);

/**
 * @brief Takes a parameter, checks if it is numeric and if it is,
 * converts it to an integer
 *
 * @param p [IN] Parameter to convert
 * @param output [OUT] integer value of given parameter
 * @return true Conversion successful
 * @return false Conversion failed
 */
bool paramToInt(const AsyncWebParameter* const p, int32_t& output);

/**
 * @brief Creates a JSON string containing the requested log messages
 * and saves it in the response stream. Inclusive indexing is used,
 * e.g. from=0 and to=3 will return the 4 oldest log messages.
 * from=-1 and to=-3 will return the 3 newest messages.
 *
 * @param response [OUT] Output response stream which will save the JSON string
 * @param from [IN] lower index of the log messages.
 * @param to [IN] upper index of the log messages.
 * @return true Successfully created JSOn string
 * @return false One index is negative while the other is positive
 */
bool getLogMessagesFromTo(AsyncResponseStream* response, int32_t from, int32_t to);

/**
 * @brief Takes a webserver request, checks if it has a 'from' and a 'to'
 * parameter and extracts their integer values.
 *
 * @param request [IN] Request to check
 * @param fromIdx [OUT] Output of the integer value of the 'from' parameter
 * @param toIdx [OUT] Output of the integer value of the 'to' parameter
 * @return true Parameters exist and conversion was successful
 * @return false Parameters don't exist or don't hold a numeric value
 */
bool getFromToIndices(const AsyncWebServerRequest* const request, int32_t& fromIdx, int32_t& toIdx);

/**
 * @brief Creates a JSON string containing various system status informations
 *
 * @param response [OUT] Output response stream which will save the JSON string
 */
void getSystemInfo(AsyncResponseStream* response);

/**
 * @brief Creates a JSON string containing non-secret configuration options
 *
 * @param response [OUT] Output response stream which will save the JSON string
 */
void getConfig(AsyncResponseStream* response);

/**
 * @brief Creates a JSON string containing sensor values which are read freshly
 * from the sensors.
 *
 * @param obj [INOUT] JsonObject to which the current sensor values will be added
 */
void getCurrentSensorData(JsonObject& obj);

#endif  // WEBSERVER_HELPERS_H