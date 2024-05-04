#include "SimpleMovingAverageFilter.h"

SimpleMovingAverageFilter::SimpleMovingAverageFilter(uint32_t n, std::shared_ptr<Transformer> next)
:Transformer(next), m_bufferSize(n){
    m_averageBuffer = new uint16_t[n];
    for(uint32_t i = 0; i < n; i++){
        m_averageBuffer[i] = 0;
    }
}

SimpleMovingAverageFilter::~SimpleMovingAverageFilter(){
    delete[] m_averageBuffer;
}

uint32_t SimpleMovingAverageFilter::transform(uint32_t input){
    // On the first call to this objects filter function
    // the buffer is completely filled with the first given
    // value for a baseline average 
    if(!m_bufferInitialized){
        m_bufferInitialized = true;
        for(uint32_t i = 0; i < m_bufferSize; i++){
            m_averageBuffer[i] = input;
        }
    }

    // add value to averaging buffer
    m_averageBuffer[m_nextBufferPosition++] = input;
    if(m_nextBufferPosition % m_bufferSize == 0) m_nextBufferPosition = 0;

    // Sum up values
    uint32_t sum = 0;
    for(uint32_t i = 0; i < m_bufferSize; i++){
        sum += m_averageBuffer[i];
    }

    // divide by number of items
    if(sum != 0) return sum / m_bufferSize;
    else return 0;
}