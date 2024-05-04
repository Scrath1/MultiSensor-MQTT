#ifndef REMAPPER_H
#define REMAPPER_H
#include "Transformer.h"

/**
 * Implementation of a range remapper to transform e.g. values from the range
 * 0-65535 to 0-100
 */
class Remapper : public Transformer{
public:
    /**
     * Constructs a Remapper object which can map values from one range to another
     * @param inMin [IN] Minimum value of input range
     * @param inMax [IN] Maximum value of input range
     * @param outMin [IN] Minimum value of output range
     * @param outMax [IN] Maximum value of output range
     * @param next [IN] shared pointer to next step in transformation pipeline.
     *  Defaults to a nullptr.
     */
    Remapper(uint32_t inMin, uint32_t inMax, uint32_t outMin, uint32_t outMax,
             std::shared_ptr<Transformer> next = std::shared_ptr<Transformer>());

protected:

    /**
     * Applies the remapping implementation
     * @param input
     * @return
     */
    uint32_t transform(uint32_t input) override;

private:
    /**
     * Variables to store the remap range configuration
     */
    const uint32_t m_inMin, m_inMax, m_outMin, m_outMax;
};

#endif //REMAPPER_H
