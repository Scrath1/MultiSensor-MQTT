#ifndef NO_FILTER_H
#define NO_FILTER_H
#include <memory>
#include "Filter.h"

class NoFilter :public Filter {
   public:
    static std::shared_ptr<Filter> instance;
    inline uint16_t filter(uint16_t input){return input;}
};

#endif  // NO_FILTER_H