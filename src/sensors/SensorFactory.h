#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

// Add new transformer implementations here
#include "transformers/SimpleMovingAverageFilter.h"
#include "transformers/Remapper.h"

// Add new sensor implementations here
#include "ADCSensor.h"
#include "RandomSensor.h"

class SensorFactory{
    private:
    /**
     * @brief Attempts to create a Remapper based on the given configuration string
     * 
     * @param configStr [INOUT] String containing key-value pairs for inMin, inMax, outMin and outMax.
     *  This string will be modified but is not guaranteed to be fully emptied
     * @return std::shared_ptr<Transformer> shared pointer to remapper object or nullptr on failure
     *  to extract the required parameters
     */
    static std::shared_ptr<Transformer> createRemapperFromStr(char configStr[]){
        char tmp[64];
        char* e;
        trimLeadingWhitespace(configStr);
        float_t inMin{0}, inMax{0}, outMin{0}, outMax{0};

        // Parse the float parameter values
        RC_t err = readKeyValueFloat(configStr, "inMin", inMin, true);
        if(RC_SUCCESS != err) return nullptr;
        err = readKeyValueFloat(configStr, "inMax", inMax, true);
        if(RC_SUCCESS != err) return nullptr;
        err = readKeyValueFloat(configStr, "outMin", outMin, true);
        if(RC_SUCCESS != err) return nullptr;
        err = readKeyValueFloat(configStr, "outMax", outMax, true);
        if(RC_SUCCESS != err) return nullptr;

        return std::make_shared<Remapper>(inMin, inMax, outMin, outMax);
    }

    /**
     * @brief Attempts to create a SimpleMovingAverageFilter based on the given configuration string
     * 
     * @param configStr [INOUT] String containing key-value pairs for the filter parameter n.
     *  This string will be modified but is not guaranteed to be fully emptied
     * @return std::shared_ptr<Transformer> shared pointer to filter object or nullptr on failure
     *  to extract the required parameters
     */
    static std::shared_ptr<Transformer> createSimpleMovingAverageFromStr(char configStr[]){
        char tmp[64];
        char* e;
        trimLeadingWhitespace(configStr);

        float_t n{0};
        RC_t err = readKeyValueFloat(configStr, "n", n, true);
        if(RC_SUCCESS != err) return nullptr;

        return std::make_shared<SimpleMovingAverageFilter>(static_cast<uint32_t>(n));
    }

    /**
     * Attempts to parse a transformer chain in the following format
     *
     * Transformer2{
     *  parameter1: value
     * }
     * Transformer1{
     *  parameter1: value
     *  parameter2: value
     *  ...
     * }
     *
     * This function only separates the transformer type string and the config string
     * for each individual transformer and then calls the function transformerFromConfigStr
     * for the individual parameter parsing implementations
     *
     * @param configStr [INOUT] String containing the transformer chain config
     * @return
     */
    static std::shared_ptr<Transformer> parseTransformerChainFromConfigStr(char configStr[]){
        std::shared_ptr<Transformer> prevTransformer;
        // Loop until no new transformer is found in string
        while(true){
            // Remove possible leading whitespaces
            trimLeadingWhitespace(configStr);
            char transformerTypeStr[128];
            uint32_t typenameLen = strcspn(configStr, TRANSFORMER_CFG_OPEN_CHAR);
            // If typenameLen is the same as the string length, no transformer name could
            // be found and the parsing is finished
            if(typenameLen == strlen(configStr)) break;

            // Copy the transformer type string to a separate string and remove it from configStr
            strncpy(transformerTypeStr, configStr, typenameLen);
            transformerTypeStr[typenameLen] = '\0';
            memmove(configStr, configStr + typenameLen, strlen(configStr + typenameLen) + 1);

            // separate the parameter part between the braces into an extra string
            char transformerCfgStr[256];
            uint32_t transformerCfgLen = strcspn(configStr, TRANSFORMER_CFG_CLOSE_CHAR);
            strncpy(transformerCfgStr, configStr, transformerCfgLen + 1);
            transformerCfgStr[transformerCfgLen] = '\0';
            memmove(configStr, configStr + transformerCfgLen + 1, strlen(configStr + transformerCfgLen) + 1);

            // Delegate actual transformer parameter parsing to individual specialized functions
            std::shared_ptr<Transformer> transformer = transformerFromConfigStr(transformerTypeStr,
                                                                                transformerCfgStr);
            // If the transformer was not created successfully, return a nullptr to indicate an error
            if(transformer == nullptr) return nullptr;
            // Chain the created transformers together
            transformer->setNextTransformer(prevTransformer);
            prevTransformer = transformer;
        }
        return prevTransformer;
    }

    /**
     * Attempts to parse a RandomSensor configuration and its transformers from a string
     * @param configStr [INOUT] String containing the config. This will be modified.
     * @return Sensor* Created sensor object or nullptr if there was an error with the configStr
     */
    static Sensor* createRandomSensorFromStr(char configStr[]){
        char name[SENSOR_NAME_MAX_LENGTH] = "";
        RC_t err = readKeyValue(configStr, "name", name, SENSOR_NAME_MAX_LENGTH, true);
        if(err != RC_SUCCESS) return nullptr;

        float_t lowerBound{0}, upperBound{0};
        err = readKeyValueFloat(configStr, "lowerBound", lowerBound, true);
        if(RC_SUCCESS != err) return nullptr;

        err = readKeyValueFloat(configStr, "upperBound", upperBound, true);
        if(RC_SUCCESS != err) return nullptr;

        // After parsing parameters, now parse the transformers
        std::shared_ptr<Transformer> transformer = parseTransformerChainFromConfigStr(configStr);
        return createRandomSensor(name, lowerBound, upperBound, transformer);
    }

    public:
    /**
     * @brief Creates a dynamically allocated ADCSensor object
     * 
     * @param name [IN] Sensor name
     * @param pin [IN] Analog pin at which the sensor is connected
     * @param transformer [IN] Optional transformer chain for
     *  processing raw sensor reading
     * @return Sensor* 
     */
    static Sensor* createADCSensor(char name[], uint32_t pin,
        std::shared_ptr<Transformer> transformer = nullptr){
        return new ADCSensor(name, pin, transformer);
    }

    /**
     * @brief Creates a dynamically allocated RandomSensor object
     * which can output randomly generated sensor values for
     * testing purposes
     * 
     * @param name [IN] Sensor name
     * @param lowerBound [IN] Lower bound of random values
     * @param upperBound [IN] Upper bound of random values
     * @param transformer [IN] Optional transformer chain for
     *  processing raw sensor reading
     * @return Sensor* 
     */
    static Sensor* createRandomSensor(char name[], float_t lowerBound, float_t upperBound, std::shared_ptr<Transformer> transformer = nullptr){
        return new RandomSensor(name, lowerBound, upperBound, transformer);
    }

    /**
     * @brief Calls the appropriate Transformer creation function based on the given transformerType string.
     * The string should match the class name of the wanted Transformer implementation.
     * 
     * @param transformerType [IN] Class name of wanted transformer implementation
     * @param configStr [INOUT] Config string containing required parameter key-value pairs.
     *  This string will be modified during parameter parsing.
     * @return std::shared_ptr<Transformer> shared pointer to created object or nullptr on failure
     *  to extract the required parameters
     */
    static std::shared_ptr<Transformer> transformerFromConfigStr(const char transformerType[], char configStr[]){
        if(strcmp(transformerType, "Remapper") == 0){
            return createRemapperFromStr(configStr);
        }
        else if(strcmp(transformerType, "SimpleMovingAverageFilter") == 0){
            return createSimpleMovingAverageFromStr(configStr);
        }
        else return nullptr;
    }

    /**
     * @brief Parses sensor config strings and attempts to create the corresponding sensor
     * @param sensorType [IN] String containing the name of the sensor type, e.g. RandomSensor
     * @param sensorTypeLen [In] Length of sensorType string
     * @param configStr [IN] String containing the entire sensor configuration
     * @return Sensor*
     */
    static Sensor* sensorFromConfigString(char sensorType[], char configStr[]){
        if(strcmp(sensorType, "RandomSensor") == 0){
            return createRandomSensorFromStr(configStr);
        }
        else return nullptr;
    }
};

#endif // SENSOR_FACTORY_H