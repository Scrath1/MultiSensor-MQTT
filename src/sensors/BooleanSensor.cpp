#include "BooleanSensor.h"

BooleanSensor::BooleanSensor(char name[], uint32_t pin, PinMode pinmode, std::shared_ptr<Transformer> transformer)
    : Sensor(name, transformer), m_gpioPin(pin) {
    pinMode(m_gpioPin, pinmode);
}

float_t BooleanSensor::readSensorRaw() { return digitalRead(m_gpioPin); }