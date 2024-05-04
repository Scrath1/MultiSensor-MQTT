#ifndef SENSOR_H
#define SENSOR_H
#include <memory>
#include "../transformers/Transformer.h"

/**
 * @brief Abstract sensor base class. Deriving classes must implement the readSensorRaw
 * function which is called by the public interface function readSensor
 * 
 */
class Sensor {
   public:
    /**
     * @brief Construct a new Sensor object
     *
     * @param filter [IN] Pointer to optional data transformation pipeline
     */
    Sensor(std::shared_ptr<Transformer> transformer = nullptr);

    /**
     * @brief Function to call for reading the sensor.
     * Will return a reading of the sensor that has been put through the filter function
     * of the sensor object. Digital values are mapped to either 0 or UINT16_MAX while
     * analog values are not remapped
     * 
     * @return SensorResult_t 
     */
    float_t readSensor();

   protected:
    /**
     * @brief Shared pointer to optional transformer pipeline
     * which is applied to any value read from the sensor.
     * Can be a nullptr
     */
    std::shared_ptr<Transformer> m_transformer;

    /**
     * @brief Returns a raw reading of the given sensor without any filtering
     * or processing.
     * This function has to be implemented by derived classes for them to work
     * 
     * @return float_t 
     */
    virtual float_t readSensorRaw() = 0;
};

#endif  // SENSOR_H