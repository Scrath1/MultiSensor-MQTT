#ifndef SENSOR_H
#define SENSOR_H
#include <memory>
#include "../filters/NoFilter.h"

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
     * @param filter [IN] Pointer to filter object which should be used for sensor
     * value filtering
     */
    Sensor(std::shared_ptr<Filter> filter = NoFilter::instance);

    /**
     * @brief Function to call for reading the sensor.
     * Will return a reading of the sensor that has been put through the filter function
     * of the sensor object. Digital values are mapped to either 0 or UINT16_MAX while
     * analog values are not remapped
     * 
     * @return uint16_t 
     */
    uint16_t readSensor();

   protected:
    /**
     * @brief Shared pointer to filter implementation
     * which is applied to any value read from the sensor
     */
    std::shared_ptr<Filter> m_filter;

    /**
     * @brief Returns a raw reading of the given sensor without any filtering
     * This function has to be implemented by derived classes for them to work
     * 
     * @return uint16_t 
     */
    virtual uint16_t readSensorRaw() = 0;
};

#endif  // SENSOR_H