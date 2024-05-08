#include "Sensor.h"

Sensor::Sensor(char name[], std::shared_ptr<Transformer> transformer) : m_transformer(transformer) {
    strncpy(m_sensorName, name, SENSOR_NAME_MAX_LENGTH - 1);
    m_sensorName[SENSOR_NAME_MAX_LENGTH] = '\0';
}

float_t Sensor::readSensor() {
    float_t rawReading = readSensorRaw();
    if (m_transformer != nullptr) {
        float_t transformedReading = m_transformer->applyTransformations(rawReading);
        return transformedReading;
    } else
        return rawReading;
}