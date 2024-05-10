#include "DHT22.h"

DHT22::DHT22(char name[], uint32_t pin, Type type,
        std::shared_ptr<Transformer> transformer)
    : Sensor(name, transformer), m_type(type), m_dht(DHT_Unified(pin, 22)){
        // the begin function also sets the pinMode
        m_dht.begin();
    }

float_t DHT22::readSensorRaw(){
    sensors_event_t event;
    float_t val = 0;
    switch(m_type){
        case TEMPERATURE:
            m_dht.temperature().getEvent(&event);
            val = event.temperature;
            break;
        case HUMIDITY:
            m_dht.humidity().getEvent(&event);
            val = event.relative_humidity;
            break;
        default:
            break;
    }
    return (isnan(val) ? 0: val);
}