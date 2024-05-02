#include "ADCSensor.h"
#include "global.h"

ADCSensor::ADCSensor(uint32_t pin, Filter& filter):Sensor(filter), m_adcPin(pin){}

uint16_t ADCSensor::readSensorRaw(){
    return analogRead(m_adcPin);
}