#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

#include "../filters/NoFilter.h"

// Add new sensor implementations here
#include "ADCSensor.h"

class SensorFactory{
    public:
    /**
     * @brief Creates a dynamically allocated ADCSensor object
     * 
     * @param pin 
     * @param filterFunc 
     * @return Sensor* 
     */
    static Sensor* createADCSensor(uint32_t pin, Filter& filter = NoFilterInstance){
        return new ADCSensor(pin, filter);
    }
};

#endif // SENSOR_FACTORY_H