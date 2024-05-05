#include "RandomSensor.h"
#include <random>

RandomSensor::RandomSensor(char name[], float_t lowerBound, float_t upperBound, std::shared_ptr<Transformer> transformer): Sensor(name, transformer), m_lowerBound(lowerBound), m_upperBound(upperBound){};

float_t RandomSensor::readSensorRaw(){
    // Generate random number between given lower and upper bound
    return m_lowerBound + static_cast<float_t>(rand())
        /(static_cast <float> (RAND_MAX/(m_upperBound-m_lowerBound)));
}