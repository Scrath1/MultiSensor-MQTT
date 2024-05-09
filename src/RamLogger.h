//
// Created by tobias on 05.11.2023.
//

#ifndef RAMLOGGER_H
#define RAMLOGGER_H

#if defined(ARDUINO)
#include <Arduino.h>
#else
#include <cstdint>
#endif
#include <cstdarg>

#include "global.h"

class RamLogger{
public:
    /**
     * @brief Constructs an RamLogger object
     * @param maxMessages [IN] Defines how many messages the buffer can store
     * @param maxMsgLen [IN] Defines how long each message can be at most, including null-terminator
     */
    RamLogger(uint32_t maxMessages, uint32_t maxMsgLen);

    /**
     * @brief Destructor
     */
    ~RamLogger();

    /**
     * @brief Checks whether anything has been saved in the logger
     * @return true or false
     */
    bool isEmpty() const;

    /**
     * @brief Checks whether the buffer has filled up completely.
     * If it has, it will overwrite the oldest entries when calling log
     * @return true or false
     */
    bool isFull() const;

    /**
     * @brief Returns the number of free elements remaining in the buffer
     * @return
     */
    uint32_t remaining() const;

    /**
     * @brief Returns the number of elements currently stored in the buffer
     * @return
    */
    uint32_t available() const;

    /**
     * @brief Appends a new string to the buffer. If the buffer is full,
     * the oldest entry will be overwritten. Will also print the string to the chosen output
     * @param str [IN] String to append to the buffer
     * @return RC_SUCCESS
     */
    RC_t log(const char str[]);

    /**
     * @brief Similar to log function but allows formatting like in printf
     * 
     * @param format [IN] Format string
     * @param ... [IN] Additional parameters as required by the format string
     * @return RC_t RC_SUCCESS on success, RC_ERROR if the formatting failed
     * In both cases the created string will be logged, even if it is malformed
     */
    RC_t logf(const char * format, ...);

    /**
     * @brief Similar to log function but allows formatting like in printf
     * 
     * @param format [IN] Format string
     * @param args [IN] Additional parameters as required by the format string
     * @return RC_t RC_SUCCESS on success, RC_ERROR if the formatting failed
     * In both cases the created string will be logged, even if it is malformed
     */
    RC_t logf(const char * format, va_list args);

    /**
     * @brief Wrapper around log which prints a line ending after the given string
     * @param str [IN] string which will be printed in a line
     * @return See log function
     */
    RC_t logLn(const char str[]);

    /**
     * @brief Similar to logLn but allows formatting like in printf
     * 
     * @param format [IN] Format string
     * @param ... [IN] Additional parameters as required by the format string
     * @return RC_t Same as in logf
     */
    RC_t logLnf(const char * format, ...);

    /**
     * @brief Returns the string at the given index
     * @param idx [IN] index of element. Can be indexed with positive or negative values.
     * 0 is equivalent to the oldest element. -1 to the newest.
     * @param str [IN] output parameter where the string will be stored
     * @param strLen [IN] size of str in bytes, including null-terminator
     * @return RC_ERROR_BUFFER_EMPTY if no elements are in the buffer,
     * RC_ERROR_BAD_PARAM if the given idx is larger than the buffer allows,
     * RC_ERROR_MEMORY if strLen indicates a too small string to hold the output
     * RC_SUCCESS on success
     */
    RC_t get(int32_t idx, char str[], uint32_t strLen) const;

    /**
     * @brief Returns the string at the given index
     * @param idx [IN] index of element. Can be indexed with positive or negative values.
     * 0 is equivalent to the oldest element. -1 to the newest.
     * @param str [IN] output parameter where the string will be stored
     * @param strLen [IN] size of str in bytes, including null-terminator
     * @param msgId [IN] output parameter which contains the number of the message
     * @return RC_ERROR_BUFFER_EMPTY if no elements are in the buffer,
     * RC_ERROR_BAD_PARAM if the given idx is larger than the buffer allows,
     * RC_ERROR_MEMORY if strLen indicates a too small string to hold the output
     * RC_SUCCESS on success
     */
    RC_t get(int32_t idx, char str[], uint32_t strLen, uint32_t& msgId) const;

    /**
     * @brief Returns the maximum length of a single message, excluding the null terminator
     * @return
     */
    uint32_t getMaxMsgLen() const;

    /**
     * @brief Returns how many strings can be stored at most before subsequent
     * log calls remove the oldest entries
     * @return
    */
    uint32_t getMaxNumEntries() const;

    /**
     * @brief Returns how many messages have passed through this buffer
     * @return
    */
    uint32_t getMsgCounter() const;

    /**
     * @brief Returns the oldest string in the buffer and erases its entry
     * @param str [IN] output parameter where the string will be stored
     * @param strLen [IN] size of str in bytes, excluding null-terminator
     * @return RC_ERROR_BUFFER_EMPTY if no elements are in the buffer,
     * RC_ERROR_BAD_PARAM if the given idx is larger than the buffer allows,
     * RC_ERROR_MEMORY if strLen indicates a too small string to hold the output
     * RC_SUCCESS on success
     */
    RC_t pop(char str[], uint32_t strLen);

    /**
     * @brief Clears the entire buffer
    */
    void clear();
private:
    /**
     * @brief Item stored in RamLogger buffer
     * 
     */
    typedef struct{
        uint32_t msgLen; /**<@brief Denotes maximum size of msg string */
        char* msg; /**<@brief Log message */
    } BufferEntry_t;
    // Array for storing log messages
    BufferEntry_t* buffer;
    // number of items the buffer can hold
    const uint32_t bufferSize;
    // length of individual items, including the null terminator
    const uint32_t msgLen;
    // Position of newest entry+1
    volatile uint32_t head = 0;
    // Position of oldest entry, aka the offset beginning of the array
    volatile uint32_t tail = 0;
    // Total number of messages that have been saved, including those that have since been deleted
    uint32_t msgCounter = 0;
    // elements currently stored in buffer. Maxes out at len
    uint32_t elementsInBuffer = 0;
};



#endif //RAMLOGGER_H
