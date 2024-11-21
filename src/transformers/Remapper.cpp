#include "Remapper.h"

Remapper::Remapper(float_t inMin, float_t inMax, float_t outMin, float_t outMax, std::shared_ptr<Transformer> next)
    : m_inMin(inMin), m_inMax(inMax), m_outMin(outMin), m_outMax(outMax) {}

float_t Remapper::transform(float_t input) {
    if(input > m_inMax) input = m_inMax;
    if(input < m_inMin) input = m_inMin;
    return (input - m_inMin) * (m_outMax - m_outMin) / (m_inMax - m_inMin) + m_outMin;
}
