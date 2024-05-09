//
// Created by tobias on 05.11.2023.
//
#include "RamLogger.h"

#include <cstring>

#if defined(ARDUINO)
// Define here how strings should be printed
#define PRINT(x) Serial.print(x)
// This should only print a line-feed
#define PRINT_LF() Serial.println("")
#else
#include <iostream>
#define PRINT(x) std::cout << x
#define PRINT_LF() std::cout << std::endl
#endif

RamLogger::RamLogger(uint32_t maxMessages, uint32_t maxMsgLen) : bufferSize(maxMessages), msgLen(maxMsgLen) {
    buffer = new BufferEntry_t[maxMessages];
    for(uint32_t i = 0; i < bufferSize; i++){
        buffer[i].msg = new char[maxMsgLen];
        buffer[i].msgLen = maxMsgLen;
        memset(buffer[i].msg, '\0', maxMsgLen);
    }
};

RamLogger::~RamLogger(){
    for(uint32_t i = 0; i < bufferSize; i++){
        delete[] buffer[i].msg;
    }
    delete[] buffer;
}

bool RamLogger::isEmpty() const {
    return (head == tail && strlen(buffer[tail].msg) == 0);
}

bool RamLogger::isFull() const {
    return (head == tail && strlen(buffer[tail].msg) != 0);
}

uint32_t RamLogger::remaining() const {
    return bufferSize - elementsInBuffer;
}

uint32_t RamLogger::available() const {
    return elementsInBuffer;
}

RC_t RamLogger::log(const char str[]) {
    // If the buffer is not completely filled yet, increment element counter
    if (!isFull()) elementsInBuffer++;
    // Find index of current element in buffer where str will be stored
    uint32_t startIdx = head;
    // if head has caught up to tail, increment tail since the oldest entry will be overwritten now
    if (head == tail && !isEmpty()) tail = (tail + 1) % (bufferSize);
    head = (head + 1) % (bufferSize);

    // copy str and increment msgCounter
    strncpy(buffer[startIdx].msg, str, msgLen);
    msgCounter++;

    PRINT(str);
    return RC_SUCCESS;
}

RC_t RamLogger::logf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    RC_t err = RamLogger::logf(format, args);
    va_end(args);
    return err;
}

RC_t RamLogger::logf(const char* format, va_list args) {
    char* buf = new char[msgLen];
    // n contains the number of bits written if buffer is sufficiently large
    // not counting the null-terminator
    // on an encoding error, it receives a negative value
    int n = vsnprintf(buf, msgLen, format, args);
    RamLogger::log(buf);
    delete[] buf;
    if (n < msgLen-1 && n >= 0) {
        return RC_SUCCESS;
    }
    return RC_ERROR;
}

RC_t RamLogger::logLn(const char str[]) {
    RC_t err = log(str);
    PRINT_LF();
    return err;
}

RC_t RamLogger::logLnf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    RC_t err = RamLogger::logf(format, args);
    va_end(args);
    PRINT_LF();
    return err;
}

RC_t RamLogger::get(int32_t idx, char str[], uint32_t strLen) const {
    if (isEmpty()) return RC_ERROR_BUFFER_EMPTY;
    // Check that idx is in bounds.
    // e.g. there is 1 element in the buffer,
    // the index may not be 1 or -2
    if (idx >= available() && idx < available() * -1) return RC_ERROR_BAD_PARAM;
    if (strLen < msgLen) return RC_ERROR_MEMORY;
    uint32_t startIdx;
    if (idx < 0) {
        // negative indexing. Starts from newest
        // Calculate the absolute idx
        idx = (idx + head + bufferSize - remaining()) % (bufferSize - remaining());
        // Get the element start idx based on the absolute idx
        startIdx = idx;
    } else {
        // positive indexing. Starts from oldest
        startIdx = tail + idx;
    }

    strncpy(str, buffer[startIdx].msg, msgLen);

    return RC_SUCCESS;
}

RC_t RamLogger::get(int32_t idx, char str[], uint32_t strLen, uint32_t& msgId) const {
    RC_t err = get(idx, str, strLen);
    if (RC_SUCCESS == err) {
        // calculate number of message
        if (idx < 0) {
            msgId = msgCounter + idx;
        } else {
            // Assume there have been 3 messages stored and the maximum entries
            // are 3. The index 1 would refer to the message with the number 1.
            // Offset = 3 - 1 - 1 = 1
            uint32_t msgCounterOffset = available() - 1 - idx;
            // Number = 3 - 1 - 1 = 1
            msgId = msgCounter - msgCounterOffset - 1;
        }
    }
    return err;
}

RC_t RamLogger::pop(char str[], uint32_t strLen) {
    // Retrieve last element
    const RC_t err = (RC_t)get(0, str, strLen);
    if (RC_SUCCESS == err) {
        elementsInBuffer--;
        // Erase popped element and increment tailcounter
        memset(buffer[tail].msg, 0, msgLen);
        tail = (tail + 1) % bufferSize;
    }
    return err;
}

uint32_t RamLogger::getMaxMsgLen() const {
    return msgLen;
}

uint32_t RamLogger::getMaxNumEntries() const {
    return bufferSize;
}

uint32_t RamLogger::getMsgCounter() const {
    return msgCounter;
}

void RamLogger::clear() {
    for(uint32_t i = 0; i < bufferSize; i++){\
        memset(buffer[i].msg, '\0', msgLen);
    }
    elementsInBuffer = 0;
}