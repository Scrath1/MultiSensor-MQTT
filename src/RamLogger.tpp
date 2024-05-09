//
// Created by tobias on 05.11.2023.
//
#include <cstring>
#include <cstdio>

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
bool RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::isEmpty() const {
    return (head == tail && strlen(buffer[tail].msg) == 0);
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
bool RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::isFull() const {
    return (head == tail && strlen(buffer[tail].msg) != 0);
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
uint32_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::remaining() const {
    return bufferSize - elementsInBuffer;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
uint32_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::available() const {
    return elementsInBuffer;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
RC_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::log(const char str[]) {
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

    // get timestamp if a function was supplied by user
    getTimestamp(buffer[startIdx].time);

    if(timestampFunction != nullptr){
        printFunction(buffer[startIdx].time);
        printFunction(": ");
    }
    printFunction(str);
    return RC_SUCCESS;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
RC_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::logf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    RC_t err = RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::logf(format, args);
    va_end(args);
    return err;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
RC_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::logf(const char* format, va_list args) {
    char* buf = new char[msgLen];
    // n contains the number of bits written if buffer is sufficiently large
    // not counting the null-terminator
    // on an encoding error, it receives a negative value
    int n = vsnprintf(buf, msgLen, format, args);
    RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::log(buf);
    delete[] buf;
    if (n < msgLen-1 && n >= 0) {
        return RC_SUCCESS;
    }
    return RC_ERROR;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
RC_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::logLn(const char str[]) {
    RC_t err = log(str);
    printFunction("\n");
    return err;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
RC_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::logLnf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    RC_t err = RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::logf(format, args);
    va_end(args);
    printFunction("\n");
    return err;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
uint32_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::index(int32_t idx) const{
    if (idx < 0) {
        // negative indexing. Starts from newest
        // Calculate the absolute idx
        idx = (idx + head + bufferSize - remaining()) % (bufferSize - remaining());
        // Get the element start idx based on the absolute idx
        return idx;
    } else {
        // positive indexing. Starts from oldest
        return tail + idx;
    }
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
const typename RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::BufferEntry_t &
RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::get(int32_t idx, RC_t &err) const {
    if (isEmpty()){
        err = RC_ERROR_BUFFER_EMPTY;
        return buffer[0];
    };
    // Check that idx is in bounds.
    // e.g. there is 1 element in the buffer,
    // the index may not be 1 or -2
    if (idx >= available() && idx < available() * -1){
        err = RC_ERROR_BAD_PARAM;
        return buffer[0];
    }
    uint32_t startIdx = index(idx);
    err = RC_SUCCESS;
    return buffer[startIdx];
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
RC_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::get(int32_t idx, char str[], uint32_t strLen) const {
    if (isEmpty()) return RC_ERROR_BUFFER_EMPTY;
    // Check that idx is in bounds.
    // e.g. there is 1 element in the buffer,
    // the index may not be 1 or -2
    if (idx >= available() && idx < available() * -1) return RC_ERROR_BAD_PARAM;
    if (strLen < msgLen) return RC_ERROR_MEMORY;
    uint32_t startIdx = index(idx);

    strncpy(str, buffer[startIdx].msg, msgLen);

    return RC_SUCCESS;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
RC_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::get(int32_t idx, char str[], uint32_t strLen, uint32_t& msgId) const {
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

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
RC_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::pop(char str[], uint32_t strLen) {
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

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
uint32_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::getMaxMsgLen() const {
    return msgLen;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
uint32_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::getMaxNumEntries() const {
    return bufferSize;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
uint32_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::getMsgCounter() const {
    return msgCounter;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
void RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::clear() {
    for(uint32_t i = 0; i < bufferSize; i++){\
        memset(buffer[i].msg, '\0', msgLen);
    }
    elementsInBuffer = 0;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
RC_t RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::setTimestampFunction(std::function<void(char[])> func){
    if(func != nullptr){
        timestampFunction = func;
        return RC_SUCCESS;
    }
    return RC_ERROR_NULL;
}

template<uint32_t maxNumberOfMessages, uint32_t maxMessageLength, uint32_t maxTimestampStrLength>
void RamLogger<maxNumberOfMessages, maxMessageLength, maxTimestampStrLength>::getTimestamp(char str[]){
    if(timestampFunction == nullptr) return;
    timestampFunction(str);
}