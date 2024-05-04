#include "Sensor.h"

Sensor::Sensor(std::shared_ptr<Transformer> transformer):m_transformer(transformer){}

float_t Sensor::readSensor(){
    float_t rawReading = readSensorRaw();
    if(m_transformer != nullptr){
        float_t transformedReading = m_transformer->applyTransformations(rawReading);
        return transformedReading;
    }
    else return rawReading;
}