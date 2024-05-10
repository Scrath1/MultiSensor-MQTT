#ifndef DHT22_H
#define DHT22_H
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "Sensor.h"

class DHT22 : public Sensor{
    public:
    enum Type{
        TEMPERATURE,
        HUMIDITY
    };

    /**
     * @brief Creates a DHT22 sensor which can output either
     * a temperature or humidity reading based on the type parameter.
     * 
     * To get both temperature and humidity, create two DHT22 objects
     * on the same pin with a different type parameter.
     * 
     * @param name [IN] Name of the sensor
     * @param pin [IN] GPIO pin number
     * @param type [IN] Temperature or Humidity mode
     * @param transformer [IN] Pointer to optional data transformation pipeline
     */
    DHT22(char name[], uint32_t pin, Type type,
        std::shared_ptr<Transformer> transformer = nullptr);

    protected:
    float_t readSensorRaw() override;

    private:

    const Type m_type;
    DHT_Unified m_dht;
};

#endif // DHT22_H