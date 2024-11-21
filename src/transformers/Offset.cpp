#include "Offset.h"

Offset::Offset(float_t offset, std::shared_ptr<Transformer> next) : Transformer(next), m_offset(offset) {}

float_t Offset::transform(float_t input) { return input + m_offset; }