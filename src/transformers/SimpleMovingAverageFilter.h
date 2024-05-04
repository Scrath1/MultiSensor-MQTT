#ifndef SIMPLE_MOVING_AVERAGE_FILTER_H
#define SIMPLE_MOVING_AVERAGE_FILTER_H
#include "Transformer.h"

/**
 * Implementation of a Simple Moving Average Filter
 */
class SimpleMovingAverageFilter : public Transformer{
    public:
    /**
     * Constructs a SimpleMovingAverageFilter with n data points
     * @param n [IN] Number of last samples to average including the current one
     * @param next [IN] shared pointer to next step in transformation pipeline.
     *  Defaults to a nullptr.
     */
    SimpleMovingAverageFilter(uint32_t n,
                              std::shared_ptr<Transformer> next = std::shared_ptr<Transformer>());
    ~SimpleMovingAverageFilter() override;

    protected:
    /**
     * @brief Applies a SMA filter to the given input
     * 
     * @note The first call to this objects filter function after
     * the instantiation completely fills the buffer with the
     * given input to provide a baseline for the average
     * 
     * @param input 
     * @return float_t
     */
    float_t transform(float_t input) override;
    private:
    /**
     * @brief Size of the averaging buffer
     */
    const uint32_t m_bufferSize;
    /**
     * @brief Averaging buffer
     */
    float_t* m_averageBuffer;
    /**
     * @brief Position in buffer where the next
     * new element should be placed
     */
    uint32_t m_nextBufferPosition = 0;
    /**
     * @brief Inidicates whether the buffer has been initialized
     * with the first given input to the transform function
     */
    bool m_bufferInitialized = false;
};

#endif // SIMPLE_MOVING_AVERAGE_FILTER_H