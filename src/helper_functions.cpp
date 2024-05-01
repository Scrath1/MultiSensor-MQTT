#include "helper_functions.h"
#include <algorithm>
#include <math.h>

bool areEqualRel(float a, float b, float epsilon) {
    return (fabs(a - b) <= epsilon * std::max(fabs(a), fabs(b)));
}