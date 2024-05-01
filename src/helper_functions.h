#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

/**
 * @brief Performs a comparison of the floats a and b with a given epsilon
 * @note Taken from https://stackoverflow.com/questions/4548004/how-to-correctly-and-standardly-compare-floats
 * 
 * @param a 
 * @param b 
 * @param epsilon 
 * @return true 
 * @return false 
 */
bool areEqualRel(float a, float b, float epsilon);

#endif // HELPER_FUNCTIONS_H