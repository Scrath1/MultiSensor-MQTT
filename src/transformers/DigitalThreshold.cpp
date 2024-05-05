#include "DigitalThreshold.h"

DigitalThreshold::DigitalThreshold(float_t thresh, std::shared_ptr<Transformer> next):Transformer(next), m_threshold(thresh){}

float_t DigitalThreshold::transform(float_t input){
    return (input >= m_threshold) ? 1 : 0;
}