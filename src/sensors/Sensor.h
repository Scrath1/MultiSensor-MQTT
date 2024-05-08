#ifndef SENSOR_H
#define SENSOR_H
#include <memory>

#include "../transformers/Transformer.h"

/**
 * @brief Maximum length of sensor name, including null terminator
 *
 */
#define SENSOR_NAME_MAX_LENGTH 64

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
     * @param name [IN] Name of the sensor.
     *  Maximum length defined by SENSOR_NAME_MAX_LENGTH
     * @param transformer [IN] Pointer to optional data transformation pipeline
     */
    Sensor(char name[], std::shared_ptr<Transformer> transformer = nullptr);

    /**
     * @brief Function to call for reading the sensor.
     * Will return a reading of the sensor that has been put through the filter function
     * of the sensor object. Digital values are mapped to either 0 or UINT16_MAX while
     * analog values are not remapped
     *
     * @return SensorResult_t
     */
    float_t readSensor();

    /**
     * @brief Returns the name assigned to this sensor
     *
     * @return const char*
     */
    inline const char* getName() const { return m_sensorName; };

    /**
     * @brief Returns how many pipeline stages this sensor has
     *
     * @return uint32_t
     */
    inline uint32_t getNumPipelineStages() const {
        if (m_transformer != nullptr)
            return m_transformer->countRemainingPipelineStages() + 1;
        else
            return 0;
    }

   protected:
    /**
     * @brief Shared pointer to optional transformer pipeline
     * which is applied to any value read from the sensor.
     * Can be a nullptr
     */
    std::shared_ptr<Transformer> m_transformer;

    /**
     * @brief Name of the sensor
     *
     */
    char m_sensorName[SENSOR_NAME_MAX_LENGTH] = "";

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