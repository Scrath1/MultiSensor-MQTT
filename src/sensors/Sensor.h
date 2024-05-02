#ifndef SENSOR_H
#define SENSOR_H
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
     * @param filter [IN] Filter to apply to any value read from the sensor.
     *  Defaults to no filter
     */
    Sensor(Filter& filter = NoFilterInstance);

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
     * @brief Filter function which is applied to any value read from the sensor
     */
    Filter& m_filter;

    /**
     * @brief Returns a raw reading of the given sensor without any filtering
     * This function has to be implemented by derived classes for them to work
     * 
     * @return uint16_t 
     */
    virtual uint16_t readSensorRaw() = 0;
};

#endif  // SENSOR_H