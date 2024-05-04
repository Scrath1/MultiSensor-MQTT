#include "Transformer.h"

Transformer::Transformer(std::shared_ptr<Transformer> next) : m_next(next){}

Transformer::~Transformer() = default;

uint32_t Transformer::applyTransformations(uint32_t input) {
    uint32_t transformedInput = transform(input);
    if(m_next != nullptr) return m_next->applyTransformations(transformedInput);
    else return transformedInput;
}
