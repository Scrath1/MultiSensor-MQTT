#include "Remapper.h"

Remapper::Remapper(uint32_t inMin, uint32_t inMax, uint32_t outMin, uint32_t outMax,
                   std::shared_ptr<Transformer> next)
                   : m_inMin(inMin), m_inMax(inMax), m_outMin(outMin), m_outMax(outMax){}

uint32_t Remapper::transform(uint32_t input) {
    if(input > m_inMax) input = m_inMax;
    if(input < m_inMin) input = m_inMin;
    return ((int64_t)input - (int64_t)m_inMin) * ((int64_t)m_outMax - (int64_t)m_outMin) / ((int64_t)m_inMax - (int64_t)m_inMin) + (int64_t)m_outMin;
}
