#ifndef MQTT_H
#define MQTT_H
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <PubSubClient.h>

#define MQTT_TASK_NAME ("MQTT_Task")
#define MQTT_TASK_STACK_SIZE (2048)
#define MQTT_TASK_PRIORITY (LOOP_TASK_PRIORITY + 1)

extern TaskHandle_t mqttTaskHandle;
extern PubSubClient mqttClient;

/**
 * @brief MQTT Task function. Should never return.
 * The job of this function is to set up the mqtt client and periodically check
 * its connection to the server. If it got disconnected, reconnect.
 * 
 * Cycle time: MQTT_TASK_CYCLE_TIME_MS
 * 
 * @param pvParameters 
 */
void mqttTask(void* pvParameters);
#endif // MQTT_H