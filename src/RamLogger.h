//
// Created by tobias on 05.11.2023.
//

#ifndef RAMLOGGER_H
#define RAMLOGGER_H

#include <cstdint>
#include <cstdarg>
#include <functional>
#include "global.h"

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
class RamLogger{
public:
    /**
     * @brief Item stored in RamLogger buffer
     */
    struct BufferEntry_t{
        const uint32_t msgLen = maxMessageLength; /**<@brief Denotes maximum size of msg string */
        char msg[maxMessageLength] = ""; /**<@brief Log message */
        char time[maxTimestampStrLength] = "";

        BufferEntry_t& operator=(const BufferEntry_t& other) {
            strncpy(this->msg, other.msg, maxMessageLength);
            strncpy(this->time, other.time, maxTimestampStrLength);
            return *this;
        }
    };

    /**
     * @brief Constructs an RamLogger object
     */
    RamLogger() = default;

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
     * @brief Returns a reference to the buffer entry at the given index
     *
     * @param idx [IN]index of element. Can be indexed with positive or negative values.
     * 0 is equivalent to the oldest element. -1 to the newest.
     * @param err [OUT] Output parameter to store error information in.
     * @return
     */
    const BufferEntry_t& get(int32_t idx, RC_t& err) const;

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

    /**
     * @brief Sets the function which the RamLogger can call
     * to get a timestamp string. The function needs to accept
     * a char array in which the formatted string is stored
     * and a uint32_t which denotes the maximum size of the char array
     * including the null-terminator.
     *
     * @param func [IN] Function with the signature "void func(char[], uint32_t)"
     * @return RC_t RC_SUCCESS on success,
     *  RC_ERROR_NULL if func is a nullpointer
     */
    RC_t setTimestampFunction(std::function<void(char[maxTimestampStrLength])> func);

    /**
     * @brief Overwrites the default print function of the RamLogger,
     * which uses printf, with a custom user defined function
     * @param func [IN] Function for printing to terminal/serial.
     * Has to return void and accept a const char[] as parameter.
     */
    inline void setPrintFunction(std::function<void(const char[])> func){
        if(func != nullptr) printFunction = func;
    }

    /**
     * @brief Resets the overwritten print function back to the default
     */
    inline void resetPrintFunction(){
        printFunction = defaultPrintFunction;
    }

private:
    /**
     * @brief Default function used for printing by RamLogger.
     * Can be changed using setPrintFunction(...) if printf
     * is not available
     *
     * @param str [IN] string to print
     */
    static void defaultPrintFunction(const char str[]){
        printf(str);
    }

    /**
     * @brief Converts an index to its actual buffer position.
     * Required because indices can be negative to indicate
     * reverse order
     * idx 0 => oldest msg
     * idx -1 => newest msg
     *
     * @param idx [IN] Index
     * @return uint32_t
     */
    uint32_t index(int32_t idx) const;
    /**
     * @brief Wrapper around timestampFunction member variable
     * which first checks whether the function is valid.
     *
     * @param str [OUT] Array in which the timestamp string is stored
     */
    void getTimestamp(char str[maxTimestampStrLength]);
    // Array for storing log messages
    BufferEntry_t buffer[maxNumberOfMessages];
    // number of items the buffer can hold
    const uint32_t bufferSize = maxNumberOfMessages;
    // length of individual items, including the null terminator
    const uint32_t msgLen = maxMessageLength;
    // Position of newest entry+1
    volatile uint32_t head = 0;
    // Position of oldest entry, aka the offset beginning of the array
    volatile uint32_t tail = 0;
    // Total number of messages that have been saved, including those that have since been deleted
    uint32_t msgCounter = 0;
    // elements currently stored in buffer. Maxes out at len
    uint32_t elementsInBuffer = 0;
    // Function which can be called to get a timestamp string.
    // May be a nullptr if not set by user with setTimestampFunction
    std::function<void(char[maxTimestampStrLength])> timestampFunction;
    /**
     * @brief Function which is called to print something to the terminal.
     * Defaults function simply uses printf, if that does not work for
     * your platform you can implement your own function and set it using
     * setPrintFunction(...)
     */
    std::function<void(const char[])> printFunction = defaultPrintFunction;
};

#include "RamLogger.tpp"

#endif //RAMLOGGER_H
