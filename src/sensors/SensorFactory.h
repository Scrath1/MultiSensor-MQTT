#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

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
    static Sensor* createADCSensor(uint32_t pin, FilterFunction filterFunc = noFilter){
        return new ADCSensor(pin, filterFunc);
    }
};

#endif // SENSOR_FACTORY_H