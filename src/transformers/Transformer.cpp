#include "Transformer.h"

Transformer::Transformer(std::shared_ptr<Transformer> next) : m_next(next) {}

Transformer::~Transformer() = default;

float_t Transformer::applyTransformations(float_t input) {
    float_t transformedInput = transform(input);
    if (m_next != nullptr)
        return m_next->applyTransformations(transformedInput);
    else
        return transformedInput;
}

uint32_t Transformer::countRemainingPipelineStages() const {
    uint32_t counter = 0;
    std::shared_ptr<Transformer> current = m_next;
    while (current != nullptr) {
        counter++;
        current = current->m_next;
    }
    return counter;
}