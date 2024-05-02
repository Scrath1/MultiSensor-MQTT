#ifndef FILTER_H
#define FILTER_H
#include "global.h"

/**
 * @brief Abstract filter base class
 *
 */
class Filter {
   public:
    Filter() = default;

    /**
     * @brief Applies a filter to the given input value
     *
     * @param input
     * @return uint16_t
     */
    virtual uint16_t filter(uint16_t input) = 0;
};

#endif  // FILTER_H