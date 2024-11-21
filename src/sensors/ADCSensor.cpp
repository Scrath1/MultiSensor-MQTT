#include "ADCSensor.h"

#include "global.h"

ADCSensor::ADCSensor(char name[], uint32_t pin, std::shared_ptr<Transformer> transformer)
    : Sensor(name, transformer), m_adcPin(pin) {}

float_t ADCSensor::readSensorRaw() { return analogRead(m_adcPin); }