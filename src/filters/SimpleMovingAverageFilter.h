#ifndef SIMPLE_MOVING_AVERAGE_FILTER_H
#define SIMPLE_MOVING_AVERAGE_FILTER_H
#include "Filter.h"

class SimpleMovingAverageFilter : public Filter{
    public:
    SimpleMovingAverageFilter(uint32_t n);
    ~SimpleMovingAverageFilter();

    /**
     * @brief Applies a SMA filter to the given input
     * 
     * @note The first call to this objects filter function after
     * the instantiation completely fills the buffer with the
     * given input to provide a baseline for the average
     * 
     * @param input 
     * @return uint16_t 
     */
    uint16_t filter(uint16_t input);
    private:
    /**
     * @brief Size of the averaging buffer
     */
    const uint32_t m_bufferSize;
    /**
     * @brief Averaging buffer
     */
    uint16_t* m_averageBuffer;
    /**
     * @brief Position in buffer where the next
     * new element should be placed
     */
    uint32_t m_nextBufferPosition = 0;
};

#endif // SIMPLE_MOVING_AVERAGE_FILTER_H