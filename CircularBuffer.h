/**
 * @file CircularBuffer.h
 * @author Seb Madgwick
 * @brief Circular buffer.
 */

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

//------------------------------------------------------------------------------
// Includes

#include <stddef.h>
#include <stdint.h>
#include <string.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Circular buffer structure.  Structure members are used internally and
 * must not be accessed by the application.
 */
typedef struct {
    uint8_t* buffer;
    size_t bufferSize;
    int writeIndex;
    int readIndex;
} CircularBuffer;

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Returns the number of bytes available in the buffer.
 * @param circularBuffer Circular buffer.
 * @return Number of bytes available in the buffer.
 */
static inline __attribute__((always_inline)) size_t CircularBufferGetReadAvailable(CircularBuffer * const circularBuffer) {
    const int writeIndex = circularBuffer->writeIndex; // avoid asynchronous hazard
    if (writeIndex < circularBuffer->readIndex) {
        return circularBuffer->bufferSize - circularBuffer->readIndex + writeIndex;
    } else {
        return writeIndex - circularBuffer->readIndex;
    }
}

/**
 * @brief Reads data from the buffer.
 * @param circularBuffer Circular buffer.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes.
 * @return Number of bytes read.
 */
static inline __attribute__((always_inline)) size_t CircularBufferRead(CircularBuffer * const circularBuffer, void* const destination, size_t numberOfBytes) {

    // Limit number of bytes to number available
    const size_t bytesAvailable = CircularBufferGetReadAvailable(circularBuffer);
    if (numberOfBytes > bytesAvailable) {
        numberOfBytes = bytesAvailable;
    }

    // Read data
    if ((circularBuffer->readIndex + numberOfBytes) >= circularBuffer->bufferSize) {
        const size_t numberOfBytesBeforeWraparound = circularBuffer->bufferSize - circularBuffer->readIndex;
        memcpy(destination, &circularBuffer->buffer[circularBuffer->readIndex], numberOfBytesBeforeWraparound);
        const size_t numberOfBytesAfterWraparound = numberOfBytes - numberOfBytesBeforeWraparound;
        memcpy(&((uint8_t*) destination)[numberOfBytesBeforeWraparound], circularBuffer->buffer, numberOfBytesAfterWraparound);
        circularBuffer->readIndex = numberOfBytesAfterWraparound;
    } else {
        memcpy(destination, &circularBuffer->buffer[circularBuffer->readIndex], numberOfBytes);
        circularBuffer->readIndex += numberOfBytes;
    }
    return numberOfBytes;
}

/**
 * @brief Reads a byte from the buffer.  This function must only be called if
 * there are bytes available in the read buffer.
 * @param circularBuffer Circular buffer.
 * @return Byte.
 */
static inline __attribute__((always_inline)) uint8_t CircularBufferReadByte(CircularBuffer * const circularBuffer) {
    const uint8_t byte = circularBuffer->buffer[circularBuffer->readIndex];
    if (++circularBuffer->readIndex >= circularBuffer->bufferSize) {
        circularBuffer->readIndex = 0;
    }
    return byte;
}

/**
 * @brief Returns the space available in the buffer.
 * @param circularBuffer Circular buffer.
 * @return Space available in the buffer.
 */
static inline __attribute__((always_inline)) size_t CircularBufferGetWriteAvailable(CircularBuffer * const circularBuffer) {
    const int readIndex = circularBuffer->readIndex; // avoid asynchronous hazard
    if (circularBuffer->writeIndex < readIndex) {
        return (circularBuffer->bufferSize - 1) - (circularBuffer->bufferSize - readIndex) - circularBuffer->writeIndex;
    } else {
        return (circularBuffer->bufferSize - 1) - (circularBuffer->writeIndex - readIndex);
    }
}

/**
 * @brief Writes data to the buffer.
 * @param circularBuffer Circular buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
static inline __attribute__((always_inline)) void CircularBufferWrite(CircularBuffer * const circularBuffer, const void* const data, const size_t numberOfBytes) {

    // Do nothing if not enough space available
    if (numberOfBytes > CircularBufferGetWriteAvailable(circularBuffer)) {
        return;
    }

    // Write data
    if ((circularBuffer->writeIndex + numberOfBytes) >= circularBuffer->bufferSize) {
        const size_t numberOfBytesBeforeWraparound = circularBuffer->bufferSize - circularBuffer->writeIndex;
        memcpy(&circularBuffer->buffer[circularBuffer->writeIndex], data, numberOfBytesBeforeWraparound);
        const size_t numberOfBytesAfterWraparound = numberOfBytes - numberOfBytesBeforeWraparound;
        memcpy(circularBuffer->buffer, &((uint8_t*) data)[numberOfBytesBeforeWraparound], numberOfBytesAfterWraparound);
        circularBuffer->writeIndex = numberOfBytesAfterWraparound;
    } else {
        memcpy(&circularBuffer->buffer[circularBuffer->writeIndex], data, numberOfBytes);
        circularBuffer->writeIndex += numberOfBytes;
    }
}

/**
 * @brief Writes a byte to the buffer.
 * @param circularBuffer Circular buffer.
 * @param byte Byte.
 */
static inline __attribute__((always_inline)) void CircularBufferWriteByte(CircularBuffer * const circularBuffer, const uint8_t byte) {

    // Do nothing if not enough space available
    if (CircularBufferGetWriteAvailable(circularBuffer) == 0) {
        return;
    }

    // Write byte
    circularBuffer->buffer[circularBuffer->writeIndex] = byte;
    if (++circularBuffer->writeIndex >= circularBuffer->bufferSize) {
        circularBuffer->writeIndex = 0;
    }
}

/**
 * @brief Clears the buffer.
 * @param circularBuffer Circular buffer.
 */
static inline __attribute__((always_inline)) void CircularBufferClear(CircularBuffer * const circularBuffer) {
    circularBuffer->readIndex = circularBuffer->writeIndex;
}

#endif

//------------------------------------------------------------------------------
// End of file
