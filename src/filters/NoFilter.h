#ifndef NO_FILTER_H
#define NO_FILTER_H
#include "Filter.h"

class NoFilter :public Filter {
   public:
    NoFilter() = default;
    inline uint16_t filter(uint16_t input){return input;}
};

// global object that can be used by anyone since this filter does not store any data
static NoFilter NoFilterInstance;

#endif  // NO_FILTER_H