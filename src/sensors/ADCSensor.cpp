#include "ADCSensor.h"
#include "global.h"

ADCSensor::ADCSensor(uint32_t pin, std::shared_ptr<Transformer> transformer):Sensor(transformer), m_adcPin(pin){}

uint16_t ADCSensor::readSensorRaw(){
    return analogRead(m_adcPin);
}