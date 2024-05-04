#include "Sensor.h"

Sensor::Sensor(std::shared_ptr<Transformer> transformer):m_transformer(transformer){}

uint16_t Sensor::readSensor(){
    uint16_t rawReading = readSensorRaw();
    if(m_transformer != nullptr){
        uint16_t transformedReading = m_transformer->applyTransformations(rawReading);
        return transformedReading;
    }
    else return rawReading;
}