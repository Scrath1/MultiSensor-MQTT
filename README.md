# MultiSensor-MQTT


## Adding a new Transformer
1. Create a class extending the Transformer base class in the transformers folder.
2. Overload the transform function and don't forget to call the Transformer constructor.
3. Include the header file in the `SensorFactory.h` file.
4. Write a `createThisTransformerFromStr` function in the
   `SensorFactory.h` file.
   This function is expected to parse the key-value pairs from the
   config file and match them to the parameters with the same name.
   It should return a `std::shared_ptr<Transformer>` result or a nullptr if
   the parsing failed.
5. Add an if statement for your Transformer to the 
   `transformerFromConfigStr` function in `SensorFactory.h` which calls
   your create function.

## Adding a new Sensor
1. Create a class extending the Sensor base class in the sensors folder.
2. Overload the `readSensorRaw` function and don't forget to call the
   Sensor constructor.
3. Include the header file in the `SensorFactory.h` file.
4. Write a `createThisSensor` function in the `SensorFactory.h` file
   which dynamically allocates your sensor and returns it as a pointer.
5. Write a `createThisSensorFromStr` function in the
   `SensorFactory.h` file. This function is expected to parse the
   key-value pairs from the config file and match them to the parameters
   with the same name and calls the function you wrote in step 4 with those
   parameters. It should also call the `parseTransformerChainFromConfigStr`
   function to read possible transformers added to your sensor.
6. Add an if statement to the `sensorFromConfigString` function in the
   `SensorFactory.h` file which calls your function from step 5 with the
   config string.

## Credit
The webinterface design was stolen and modified from the
[Jarolift_MQTT](https://github.com/madmartin/Jarolift_MQTT) project by madmartin
because I suck at webinterfaces.