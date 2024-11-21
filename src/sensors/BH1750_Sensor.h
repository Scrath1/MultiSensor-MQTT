#ifndef BH1750_SENSOR_H
#define BH1750_SENSOR_H

#define BH1750_I2C_ADDRESS_LOW (0x23)
#define BH1750_I2C_ADDRESS_HIGH (0x5C)

#include <BH1750.h>

#include "Sensor.h"

class BH1750_Sensor : public Sensor {
   public:
    /**
     * @brief Creates a BH1750 sensor which can read the current light level
     *
     * @param name [IN] Name of the sensor
     * @param addr [IN] I2C address. If the address pin is low, this should be 0x23 or 0x5c if it is high
     * @param transformer [IN] Pointer to optional data transformation pipeline
     */
    BH1750_Sensor(char name[], uint8_t addr = BH1750_I2C_ADDRESS_LOW,
                  std::shared_ptr<Transformer> transformer = nullptr);

   protected:
    float_t readSensorRaw() override;

   private:
    BH1750 m_sensor;
};

#endif  // BH1750_SENSOR_H