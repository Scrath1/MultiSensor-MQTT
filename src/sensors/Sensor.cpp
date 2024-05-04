#include "Sensor.h"

Sensor::Sensor(std::shared_ptr<Filter> filter):m_filter(filter){}

uint16_t Sensor::readSensor(){
    uint16_t rawReading = readSensorRaw();
    return m_filter.get()->filter(rawReading);
}