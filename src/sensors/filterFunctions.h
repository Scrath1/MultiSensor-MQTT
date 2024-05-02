#ifndef FILTER_FUNCTIONS_H
#define FILTER_FUNCTIONS_H

typedef std::function<uint16_t(uint16_t)> FilterFunction;

/**
 * @brief Default filter function which is used when no other filter is selected.
 * This function directly returns the raw value without doing any filtering
 * 
 * @param val 
 * @return uint16_t 
 */
inline uint16_t noFilter(uint16_t val){return val;}

#endif // FILTER_FUNCTIONS_H