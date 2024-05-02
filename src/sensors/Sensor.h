#ifndef SENSOR_H
#define SENSOR_H
#include <functional>

#include "filterFunctions.h"

class Sensor {
   public:
    /**
     * @brief Construct a new Sensor object
     *
     * @param filterFunc [IN] Function to apply to any value read from the sensor.
     *  Defaults to no filter
     */
    Sensor(FilterFunction filterFunc = noFilter);

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
    FilterFunction m_filterFunc;

    /**
     * @brief Returns a raw reading of the given sensor without any filtering
     * This function has to be implemented by derived classes for them to work
     * 
     * @return uint16_t 
     */
    virtual uint16_t readSensorRaw() = 0;
};

#endif  // SENSOR_H