#ifndef BOOLEAN_SENSOR_H
#define BOOLEAN_SENSOR_H
#include "Sensor.h"

/**
 * @brief Sensor which outputs either 0 or 1 based on the input
 * of a digital pin
 * 
 */
class BooleanSensor: public Sensor{
public:
    // Redefinition of own type to exclude the Output mode
    enum PinMode{
        Input = INPUT,
        InputPullUp = INPUT_PULLUP,
        InputPullDown = INPUT_PULLDOWN
    };

    /**
     * @brief Creates a boolean sensor which will either output 0 or 1
     * 
     * @param name [IN] Name of the sensor
     * @param pin [IN] GPIO pin number
     * @param mode [IN] pin mode, defaults to Input
     * @param transformer [IN] Pointer to optional data transformation pipeline
     */
    BooleanSensor(char name[], uint32_t pin, PinMode pinMode = Input, 
        std::shared_ptr<Transformer> transformer = nullptr);

    protected:
    float_t readSensorRaw() override;

    private:
    const uint32_t m_gpioPin;
};

#endif // BOOLEAN_SENSOR_H