#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#include "global.h"

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

/**
 * Removes all comment strings from given str.
 * Comments are marked at the beginning AND end
 * @param str [INOUT] string to modify
 * @param commentDelimiter [IN] string with which
 *  the comment start and end are marked.
 * @return RC_SUCCESS on success
 */
RC_t trimComments(char str[], const char commentDelimiter[]);

/**
 * @brief Removes leading whitespace from the given string.
 * This includes space characters, tabs and linebreaks of either \r or \n form
 *
 * @param str [INOUT] string to modify
 * @return RC_t RC_SUCCESS on success
 */
RC_t trimLeadingWhitespace(char str[]);

/**
 * Removes the given key value pair from the string, starting with the key
 * and ending with the newline
 * @param str [INOUT] String to modify
 * @param key [IN] Key to look for
 * @return RC_SUCCESS on success,
 *  RC_ERROR_ZERO if key does not exist
 */
RC_t removeKeyValuePair(char str[], const char key[]);

/**
 * @brief Reads the value for the given key from the string.
 * Optionally can also remove the key-value pair from the string,
 * starting with the key and ending with the next newline following the value.
 *
 * @param str [INOUT] String to read from
 * @param key [IN] Key to look for
 * @param value [OUT] Output parameter for the value corresponding to the given key parameter
 * @param valueLen [IN] Maximum buffer size of value parameter
 * @param remove [IN] Optional flag to delete the read key-value pair from the string
 * @return RC_t RC_SUCCESS on success
 *  RC_ERROR_ZERO if the key was not found in the string
 *  RC_ERROR_BAD_DATA if the key was found but there is no value attached to it
 *  RC_ERROR_BUFFER_FULL if the value was found but does not fit in the provided buffer
 */
RC_t readKeyValue(char str[], const char key[], char value[], const uint32_t valueLen, bool remove = false);

/**
 * @brief Same as the function readKeyValue but attempts to convert the read value
 * into a float
 *
 * @param str [INOUT] String to read from
 * @param key [IN] Key to look for
 * @param value [OUT] Output parameter in which the successfully converted float will be stored
 * @param remove [IN] Optional flag to delete the read key-value pair from the string
 * @return RC_t RC_SUCCESS on success
 *  RC_ERROR_ZERO if the key was not found in the string
 *  RC_ERROR_BAD_DATA if the key was found but there is no value attached to it
 *  RC_ERROR_BUFFER_FULL if the value was found but does not fit in the provided buffer
 *  RC_ERROR_INVALID if the float conversion failed
 */
RC_t readKeyValueFloat(char str[], const char key[], float_t& value, bool remove = false);

/**
 * @brief Same as the function readKeyValue but attempts to convert the read value
 * into an integer
 *
 * @param str [INOUT] String to read from
 * @param key [IN] Key to look for
 * @param value [OUT] Output parameter in which the successfully converted int will be stored
 * @param remove [IN] Optional flag to delete the read key-value pair from the string
 * @return RC_t RC_SUCCESS on success
 *  RC_ERROR_ZERO if the key was not found in the string
 *  RC_ERROR_BAD_DATA if the key was found but there is no value attached to it
 *  RC_ERROR_BUFFER_FULL if the value was found but does not fit in the provided buffer
 *  RC_ERROR_INVALID if the int conversion failed
 */
RC_t readKeyValueInt(char str[], const char key[], int32_t& value, bool remove = false);

#endif  // HELPER_FUNCTIONS_H