#ifndef OFFSET_H
#define OFFSET_H
#include "Transformer.h"

/**
 * @brief Applies a constant offset to the given values
 *
 */
class Offset : public Transformer {
   public:
    /**
     * @brief Creates an Offset transformator which adds or subtracts a fixed number
     * from given values
     *
     * @param offset [IN] Offset to add
     * @param next [IN] shared pointer to next step in transformation pipeline.
     *  Defaults to a nullptr.
     */
    Offset(float offset, std::shared_ptr<Transformer> next = std::shared_ptr<Transformer>());

   protected:
    /**
     * @brief Applies offset transformation
     *
     * @param input
     * @return float_t
     */
    float_t transform(float_t input) override;

   private:
    const float_t m_offset;
};
#endif  // OFFSET_H