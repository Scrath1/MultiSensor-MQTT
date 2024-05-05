#ifndef DIGITAL_THRESHOLD_H
#define DIGITAL_THRESHOLD_H
#include "Transformer.h"

class DigitalThreshold : public Transformer{
    public:
    /**
     * @brief Creates a Digital Threshold transformator which converts values at and above
     * the given threshold to a 1 and values below to a 0
     * 
     * @param thresh [IN] Threshold from which values are seen as a logic 1
     * @param next [IN] shared pointer to next step in transformation pipeline.
     *  Defaults to a nullptr.
     */
    DigitalThreshold(float_t thresh, std::shared_ptr<Transformer> next = std::shared_ptr<Transformer>());

    protected:
    /**
     * @brief Applies digital threshold transformation
     * 
     * @param input 
     * @return float_t 
     */
    float_t transform(float_t input) override;

    private:
    const float_t m_threshold;
};

#endif // DIGITAL_THRESHOLD_H