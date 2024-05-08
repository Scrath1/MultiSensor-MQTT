#ifndef REMAPPER_H
#define REMAPPER_H
#include "Transformer.h"

/**
 * Implementation of a range remapper to transform e.g. values from the range
 * 0-65535 to 0-100
 */
class Remapper : public Transformer {
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
    Remapper(float_t inMin, float_t inMax, float_t outMin, float_t outMax,
             std::shared_ptr<Transformer> next = std::shared_ptr<Transformer>());

   protected:
    /**
     * Applies the remapping implementation
     * @param input
     * @return
     */
    float_t transform(float_t input) override;

   private:
    /**
     * Variables to store the remap range configuration
     */
    const float_t m_inMin, m_inMax, m_outMin, m_outMax;
};

#endif  // REMAPPER_H
