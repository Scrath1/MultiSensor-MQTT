#ifndef RANDOM_SENSOR_H
#define RANDOM_SENSOR_H
#include "Sensor.h"

/**
 * @brief Sensor for test purposes.
 * Generates random numbers within given range
 */
class RandomSensor : public Sensor{
    public:
    /**
     * @brief Construct a new Random Sensor object
     * 
     * @param name [IN] Name of the sensor
     * @param lowerBound [IN] Lowest possible number to generate
     * @param upperBound [IN] Highest possible number to generate
     * @param transformer [IN] Pointer to optional data transformation pipeline
     */
    RandomSensor(char name[], float_t lowerBound, float_t upperBound, std::shared_ptr<Transformer> transformer = nullptr);

    protected:
    float_t readSensorRaw() override;

    private:
    float_t m_lowerBound, m_upperBound;
};

#endif // RANDOM_SENSOR_H