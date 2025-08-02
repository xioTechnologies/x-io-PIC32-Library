/**
 * @file Fifo.h
 * @author Seb Madgwick
 * @brief Asynchronous FIFO buffer.
 */

#ifndef FIFO_H
#define FIFO_H

//------------------------------------------------------------------------------
// Includes

#include <stddef.h>
#include <stdint.h>
#include <string.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief FIFO structure. All structure members are private except for
 * initialisation.
 * Example:
 * @code
 * uint8_t data[1024];
 * Fifo fifo = {.data = data, .dataSize = sizeof (data)};
 * @endcode
 */
typedef struct {
    volatile uint8_t * const data;
    const size_t dataSize;
    volatile size_t writeIndex;
    volatile size_t readIndex;
} Fifo;

/**
 * @brief Result.
 */
typedef enum {
    FifoResultOk,
    FifoResultError,
} FifoResult;

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Returns the number of bytes available to read from the FIFO.
 * @param fifo FIFO structure.
 * @return Number of bytes available in the buffer.
 */
static inline __attribute__((always_inline)) size_t FifoAvailableRead(Fifo * const fifo) {
    const size_t writeIndex = fifo->writeIndex; // avoid asynchronous hazard
    if (writeIndex < fifo->readIndex) {
        return fifo->dataSize - fifo->readIndex + writeIndex;
    } else {
        return writeIndex - fifo->readIndex;
    }
}

/**
 * @brief Reads data from the FIFO.
 * @param fifo FIFO structure.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes.
 * @return Number of bytes read.
 */
static inline __attribute__((always_inline)) size_t FifoRead(Fifo * const fifo, void* const destination, size_t numberOfBytes) {

    // Do nothing if no bytes available to read
    const size_t bytesAvailable = FifoAvailableRead(fifo);
    if (bytesAvailable == 0) {
        return 0;
    }

    // Limit number of bytes to number available
    if (numberOfBytes > bytesAvailable) {
        numberOfBytes = bytesAvailable;
    }

    // Read data
    if ((fifo->readIndex + numberOfBytes) >= fifo->dataSize) {
        const size_t numberOfBytesBeforeWraparound = fifo->dataSize - fifo->readIndex;
        memcpy(destination, (void*) &fifo->data[fifo->readIndex], numberOfBytesBeforeWraparound);
        const size_t numberOfBytesAfterWraparound = numberOfBytes - numberOfBytesBeforeWraparound;
        memcpy(&((uint8_t*) destination)[numberOfBytesBeforeWraparound], (void*) fifo->data, numberOfBytesAfterWraparound);
        fifo->readIndex = numberOfBytesAfterWraparound;
    } else {
        memcpy(destination, (void*) &fifo->data[fifo->readIndex], numberOfBytes);
        fifo->readIndex += numberOfBytes;
    }
    return numberOfBytes;
}

/**
 * @brief Reads a byte from the FIFO. This function must only be called if
 * there are bytes available to read.
 * @param fifo FIFO structure.
 * @return Byte.
 */
static inline __attribute__((always_inline)) uint8_t FifoReadByte(Fifo * const fifo) {
    const uint8_t byte = fifo->data[fifo->readIndex];
    if (++fifo->readIndex >= fifo->dataSize) {
        fifo->readIndex = 0;
    }
    return byte;
}

/**
 * @brief Returns the space available to write to the FIFO.
 * @param fifo FIFO structure.
 * @return Space available in the buffer.
 */
static inline __attribute__((always_inline)) size_t FifoAvailableWrite(Fifo * const fifo) {
    const size_t readIndex = fifo->readIndex; // avoid asynchronous hazard
    if (fifo->writeIndex < readIndex) {
        return (fifo->dataSize - 1) - (fifo->dataSize - readIndex) - fifo->writeIndex;
    } else {
        return (fifo->dataSize - 1) - (fifo->writeIndex - readIndex);
    }
}

/**
 * @brief Writes data to the FIFO.
 * @param fifo FIFO structure.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @return Result.
 */
static inline __attribute__((always_inline)) FifoResult FifoWrite(Fifo * const fifo, const void* const data, const size_t numberOfBytes) {

    // Do nothing if not enough space available
    if (numberOfBytes > FifoAvailableWrite(fifo)) {
        return FifoResultError;
    }

    // Write data
    if ((fifo->writeIndex + numberOfBytes) >= fifo->dataSize) {
        const size_t numberOfBytesBeforeWraparound = fifo->dataSize - fifo->writeIndex;
        memcpy((void*) &fifo->data[fifo->writeIndex], data, numberOfBytesBeforeWraparound);
        const size_t numberOfBytesAfterWraparound = numberOfBytes - numberOfBytesBeforeWraparound;
        memcpy((void*) fifo->data, &((uint8_t*) data)[numberOfBytesBeforeWraparound], numberOfBytesAfterWraparound);
        fifo->writeIndex = numberOfBytesAfterWraparound;
    } else {
        memcpy((void*) &fifo->data[fifo->writeIndex], data, numberOfBytes);
        fifo->writeIndex += numberOfBytes;
    }
    return FifoResultOk;
}

/**
 * @brief Writes a byte to the FIFO.
 * @param fifo FIFO structure.
 * @param byte Byte.
 * @return Result.
 */
static inline __attribute__((always_inline)) FifoResult FifoWriteByte(Fifo * const fifo, const uint8_t byte) {

    // Do nothing if not enough space available
    if (FifoAvailableWrite(fifo) == 0) {
        return FifoResultError;
    }

    // Write byte
    fifo->data[fifo->writeIndex] = byte;
    if (++fifo->writeIndex >= fifo->dataSize) {
        fifo->writeIndex = 0;
    }
    return FifoResultOk;
}

/**
 * @brief Clears the FIFO.
 * @param fifo FIFO structure.
 */
static inline __attribute__((always_inline)) void FifoClear(Fifo * const fifo) {
    fifo->readIndex = fifo->writeIndex;
}

#endif

//------------------------------------------------------------------------------
// End of file
