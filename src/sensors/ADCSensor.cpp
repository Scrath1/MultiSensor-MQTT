#include "ADCSensor.h"
#include "global.h"

ADCSensor::ADCSensor(uint32_t pin, FilterFunction filterFunc):Sensor(filterFunc), m_adcPin(pin){}

uint16_t ADCSensor::readSensorRaw(){
    return analogRead(m_adcPin);
}