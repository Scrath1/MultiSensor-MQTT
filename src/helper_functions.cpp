#include "helper_functions.h"
#include <algorithm>
#include <math.h>

bool areEqualRel(float a, float b, float epsilon) {
    return (fabs(a - b) <= epsilon * std::max(fabs(a), fabs(b)));
}

RC_t trimLeadingWhitespace(char str[]){
    // Count leading spaces, tabs and linebreak characters
    uint32_t leadingWhitespaceChars = 0;
    for(uint32_t i = 0; str[i] != '\0'; i++){
        char c = str[i];
        if(c == ' ' || c == '\r' || c == '\n' || c == '\t')
            leadingWhitespaceChars++;
        else // Char is not whitespace. Stop
            break;
    }
    char* whitespaceEnd = str + leadingWhitespaceChars;
    memmove(str, whitespaceEnd, strlen(whitespaceEnd) + 1);
    return RC_SUCCESS;
}

RC_t trimComments(char str[], const char commentDelimiter[]){
    while(true){
        // get pointer to position of first comment delimiter
        char* commentStart = strstr(str, commentDelimiter);
        if(commentStart == nullptr) break;

        // from position of first comment delimiter, search until the next one.
        // unlike commentStart, commentEnd points to the first character after the delimiter
        char* commentEnd = strstr(commentStart + strlen(commentDelimiter),
            commentDelimiter);
        if(commentEnd == nullptr) break;
        else commentEnd += strlen(commentDelimiter);

        // move rest of string to former start of string to be removed
        memmove(commentStart, commentEnd, strlen(commentEnd) + 1);
    }
    return RC_SUCCESS;
}

RC_t removeKeyValuePair(char str[], const char key[]){
    char* keyStart = strstr(str, key);
    // Key was not found in string
    if(keyStart == nullptr) return RC_ERROR_ZERO;
    uint32_t kvLen = strcspn(keyStart, "\r\n");
    char lineEndChar = (keyStart+kvLen)[0];
    if(lineEndChar == '\n') kvLen = kvLen+1;
    else if(lineEndChar == '\r') kvLen = kvLen+2;

    memmove(keyStart, keyStart + kvLen, strlen(keyStart + kvLen) + 1);
    return RC_SUCCESS;
}

RC_t readKeyValue(char str[], const char key[], char value[], const uint32_t valueLen, bool remove){
    char* keyStart = strstr(str, key);
    // Key was not found in string
    if(keyStart == nullptr) return RC_ERROR_ZERO;
    char* valueStart = keyStart + strlen(key) + 1;
    // Skip over whitespace in front of value
    while(valueStart[0] == ' ' || valueStart[0] == '\t') valueStart++;
    // Count actual length of value until either \r or \n is found
    uint32_t len = strcspn(valueStart, "\r\n");
    // Check that there is actually a value and that it fits in the given buffer
    if(len == 0) return RC_ERROR_BAD_DATA;
    if(len >= valueLen) return RC_ERROR_BUFFER_FULL;
    strncpy(value, valueStart, len);
    // ensure that value is null-terminated
    value[len] = '\0';

    // if remove flag is set, delete the kv pair from the string
    if(remove){
        removeKeyValuePair(str, key);
    }

    return RC_SUCCESS;
}

RC_t readKeyValueFloat(char str[], const char key[], float_t& value, bool remove){
    char buf[32];
    RC_t err = readKeyValue(str,key, buf, sizeof(buf), remove);
    if(RC_SUCCESS != err) return err;

    // Try to convert string to float
    char* e;
    errno = 0;
    value = strtof(buf, &e);
    if(*e != '\0' || errno != 0) return RC_ERROR_INVALID;
    else return RC_SUCCESS;
}

RC_t readKeyValueInt(char str[], const char key[], int32_t& value, bool remove){
    char buf[32];
    RC_t err = readKeyValue(str,key, buf, sizeof(buf), remove);
    if(RC_SUCCESS != err) return err;

    // Try to convert string to int
    errno = 0;
    value = strtol(buf, NULL, 10);
    if(errno != 0) return RC_ERROR_INVALID;
    else return RC_SUCCESS;
}