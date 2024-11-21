#include "BH1750_Sensor.h"

#include <Wire.h>

BH1750_Sensor::BH1750_Sensor(char name[], uint8_t addr, std::shared_ptr<Transformer> transformer)
    : Sensor(name, transformer), m_sensor{addr} {
    Wire.begin();
    m_sensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, addr, &Wire);
}

float_t BH1750_Sensor::readSensorRaw() { return static_cast<float_t>(m_sensor.readLightLevel()); }