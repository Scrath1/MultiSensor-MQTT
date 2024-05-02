#include "Sensor.h"

Sensor::Sensor(Filter& filter):m_filter(filter){}

uint16_t Sensor::readSensor(){
    uint16_t rawReading = readSensorRaw();
    return m_filter.filter(rawReading);
}