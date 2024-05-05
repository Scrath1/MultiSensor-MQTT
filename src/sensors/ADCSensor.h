#ifndef ADCSENSOR_H
#define ADCSENSOR_H
#include "Sensor.h"
#include "../transformers/Transformer.h"

/**
 * @brief Class for reading a sensor connected directly to an ADC input pin
 * on the microcontroller
 * 
 */
class ADCSensor : public Sensor{
    public:
    /**
     * @brief Constructs a new ADCSensor which can read analog values from the
     * analog pins of the microcontroller
     * 
     * @param name [IN] Name of the sensor
     * @param pin [IN] Analog pin number
     * @param transformer [IN] Pointer to optional data transformation pipeline
     */
    ADCSensor(char name[], uint32_t pin, std::shared_ptr<Transformer> transformer = nullptr);

    protected:
    float_t readSensorRaw() override;

    private:
    /**
     * @brief ADC pin to which the sensor is connected
     * 
     */
    const uint32_t m_adcPin;
};

#endif // ADCSENSOR_H