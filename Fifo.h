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
 * @brief FIFO structure.  Structure members are used internally and must not
 * be accessed by the application except for initialisation.
 *
 * Example:
 * @code
 * uint8_t data[1024];
 * Fifo fifo = {.data = data, .dataSize = sizeof (data)};
 * @endcode
 */
typedef struct {
    uint8_t* data;
    size_t dataSize;
    int writeIndex;
    int readIndex;
} Fifo;

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Returns the number of bytes available to read from the FIFO.
 * @param fifo FIFO structure.
 * @return Number of bytes available in the buffer.
 */
static inline __attribute__((always_inline)) size_t FifoGetReadAvailable(Fifo * const fifo) {
    const int writeIndex = fifo->writeIndex; // avoid asynchronous hazard
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

    // Limit number of bytes to number available
    const size_t bytesAvailable = FifoGetReadAvailable(fifo);
    if (numberOfBytes > bytesAvailable) {
        numberOfBytes = bytesAvailable;
    }

    // Read data
    if ((fifo->readIndex + numberOfBytes) >= fifo->dataSize) {
        const size_t numberOfBytesBeforeWraparound = fifo->dataSize - fifo->readIndex;
        memcpy(destination, &fifo->data[fifo->readIndex], numberOfBytesBeforeWraparound);
        const size_t numberOfBytesAfterWraparound = numberOfBytes - numberOfBytesBeforeWraparound;
        memcpy(&((uint8_t*) destination)[numberOfBytesBeforeWraparound], fifo->data, numberOfBytesAfterWraparound);
        fifo->readIndex = numberOfBytesAfterWraparound;
    } else {
        memcpy(destination, &fifo->data[fifo->readIndex], numberOfBytes);
        fifo->readIndex += numberOfBytes;
    }
    return numberOfBytes;
}

/**
 * @brief Reads a byte from the FIFO.  This function must only be called if
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
 * @brief Returns the space available to write in the FIFO.
 * @param fifo FIFO structure.
 * @return Space available in the buffer.
 */
static inline __attribute__((always_inline)) size_t FifoGetWriteAvailable(Fifo * const fifo) {
    const int readIndex = fifo->readIndex; // avoid asynchronous hazard
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
 */
static inline __attribute__((always_inline)) void FifoWrite(Fifo * const fifo, const void* const data, const size_t numberOfBytes) {

    // Do nothing if not enough space available
    if (numberOfBytes > FifoGetWriteAvailable(fifo)) {
        return;
    }

    // Write data
    if ((fifo->writeIndex + numberOfBytes) >= fifo->dataSize) {
        const size_t numberOfBytesBeforeWraparound = fifo->dataSize - fifo->writeIndex;
        memcpy(&fifo->data[fifo->writeIndex], data, numberOfBytesBeforeWraparound);
        const size_t numberOfBytesAfterWraparound = numberOfBytes - numberOfBytesBeforeWraparound;
        memcpy(fifo->data, &((uint8_t*) data)[numberOfBytesBeforeWraparound], numberOfBytesAfterWraparound);
        fifo->writeIndex = numberOfBytesAfterWraparound;
    } else {
        memcpy(&fifo->data[fifo->writeIndex], data, numberOfBytes);
        fifo->writeIndex += numberOfBytes;
    }
}

/**
 * @brief Writes a byte to the FIFO.
 * @param fifo FIFO structure.
 * @param byte Byte.
 */
static inline __attribute__((always_inline)) void FifoWriteByte(Fifo * const fifo, const uint8_t byte) {

    // Do nothing if not enough space available
    if (FifoGetWriteAvailable(fifo) == 0) {
        return;
    }

    // Write byte
    fifo->data[fifo->writeIndex] = byte;
    if (++fifo->writeIndex >= fifo->dataSize) {
        fifo->writeIndex = 0;
    }
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
