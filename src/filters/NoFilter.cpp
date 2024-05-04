#include "NoFilter.h"

std::shared_ptr<Filter> NoFilter::instance = std::shared_ptr<Filter>(new NoFilter());