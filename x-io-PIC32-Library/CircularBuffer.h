/**
 * @file CircularBuffer.h
 * @author Seb Madgwick
 * @brief Circular buffer read and write functions.
 */

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

//------------------------------------------------------------------------------
// Includes

#include <stddef.h>
#include <stdint.h>
#include <string.h>

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Reads data from a circular buffer.
 * @param buffer Buffer.
 * @param bufferSize Buffer size.
 * @param bufferReadIndex Buffer read index.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes.
 */
static inline __attribute__((always_inline)) void CircularBufferRead(const uint8_t * const buffer, const size_t bufferSize, int* const bufferReadIndex, void* const destination, const size_t numberOfBytes) {
    if ((*bufferReadIndex + numberOfBytes) >= bufferSize) {
        const size_t numberOfBytesBeforeWraparound = bufferSize - *bufferReadIndex;
        memcpy(destination, &buffer[*bufferReadIndex], numberOfBytesBeforeWraparound);
        const size_t numberOfBytesAfterWraparound = numberOfBytes - numberOfBytesBeforeWraparound;
        memcpy(&((uint8_t*) destination)[numberOfBytesBeforeWraparound], buffer, numberOfBytesAfterWraparound);
        *bufferReadIndex = numberOfBytesAfterWraparound;
    } else {
        memcpy(destination, &buffer[*bufferReadIndex], numberOfBytes);
        *bufferReadIndex += numberOfBytes;
    }
}

/**
 * @brief Writes data to a circular buffer.
 * @param buffer Buffer.
 * @param bufferSize Buffer size.
 * @param bufferWriteIndex Buffer write index.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
static inline __attribute__((always_inline)) void CircularBufferWrite(uint8_t * const buffer, const size_t bufferSize, int* const bufferWriteIndex, const void* const data, const size_t numberOfBytes) {
    if ((*bufferWriteIndex + numberOfBytes) >= bufferSize) {
        const size_t numberOfBytesBeforeWraparound = bufferSize - *bufferWriteIndex;
        memcpy(&buffer[*bufferWriteIndex], data, numberOfBytesBeforeWraparound);
        const size_t numberOfBytesAfterWraparound = numberOfBytes - numberOfBytesBeforeWraparound;
        memcpy(buffer, &((uint8_t*) data)[numberOfBytesBeforeWraparound], numberOfBytesAfterWraparound);
        *bufferWriteIndex = numberOfBytesAfterWraparound;
    } else {
        memcpy(&buffer[*bufferWriteIndex], data, numberOfBytes);
        *bufferWriteIndex += numberOfBytes;
    }
}

#endif

//------------------------------------------------------------------------------
// End of file
