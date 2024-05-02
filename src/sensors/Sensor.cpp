#include "Sensor.h"

Sensor::Sensor(FilterFunction filterFunc):m_filterFunc(filterFunc){}

uint16_t Sensor::readSensor(){
    uint16_t rawReading = readSensorRaw();
    return m_filterFunc(rawReading);
}