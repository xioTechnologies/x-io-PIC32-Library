/**
 * @file Fifo.h
 * @author Seb Madgwick
 * @brief Asynchronous FIFO buffer.
 */

#ifndef FIFO_H
#define FIFO_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief FIFO structure. All structure members are private except for
 * initialisation.
 *
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
 * @brief Returns the capacity of the FIFO.
 * @param fifo FIFO structure.
 * @return Capacity of the FIFO.
 */
static inline __attribute__((always_inline)) size_t FifoCapacity(Fifo * const fifo) {
    return fifo->dataSize - 1;
}

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

    // Read data with no wraparound
    if ((fifo->readIndex + numberOfBytes) < fifo->dataSize) {
        memcpy(destination, (void*) &fifo->data[fifo->readIndex], numberOfBytes);
        fifo->readIndex += numberOfBytes;
        return numberOfBytes;
    }

    // Read data with wraparound
    const size_t numberOfBytesBeforeWraparound = fifo->dataSize - fifo->readIndex;
    memcpy(destination, (void*) &fifo->data[fifo->readIndex], numberOfBytesBeforeWraparound);
    const size_t numberOfBytesAfterWraparound = numberOfBytes - numberOfBytesBeforeWraparound;
    memcpy(&((uint8_t*) destination)[numberOfBytesBeforeWraparound], (void*) fifo->data, numberOfBytesAfterWraparound);
    fifo->readIndex = numberOfBytesAfterWraparound;
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
 * @brief Provides a pointer to the next contiguous block of data in the FIFO.
 * FifoReadPointerComplete must be called after data has been read.
 * @param fifo FIFO structure.
 * @param pointer Pointer.
 * @param numberOfBytes Number of bytes.
 */
static inline __attribute__((always_inline)) void FifoReadPointer(Fifo * const fifo, volatile void* * const pointer, size_t * const numberOfBytes) {
    *pointer = (void*) &fifo->data[fifo->readIndex];
    const size_t writeIndex = fifo->writeIndex; // avoid asynchronous hazard
    if (writeIndex < fifo->readIndex) {
        *numberOfBytes = fifo->dataSize - fifo->readIndex;
    } else {
        *numberOfBytes = writeIndex - fifo->readIndex;
    }
}

/**
 * @brief Updates the FIFO after FifoReadPointer.
 * @param fifo FIFO structure.
 * @param numberOfBytes Number of bytes.
 */
static inline __attribute__((always_inline)) void FifoReadPointerComplete(Fifo * const fifo, const size_t numberOfBytes) {
    fifo->readIndex += numberOfBytes;
    if (fifo->readIndex >= fifo->dataSize) {
        fifo->readIndex = 0;
    }
}

/**
 * @brief Returns the size of the next packet in the FIFO. This function must
 * only be used if all data in the FIFO was written using FifoWritePacket.
 * @param fifo FIFO structure.
 * @return Size of the next packet.
 */
static inline __attribute__((always_inline)) size_t FifoAvailableReadPacket(Fifo * const fifo) {
    if (FifoAvailableRead(fifo) == 0) {
        return 0;
    }
    if (fifo->readIndex == fifo->dataSize - 1) {
        fifo->readIndex = 0; // packetSize will never be written to wrapround
    }
    volatile void *unaligned;
    size_t numberOfBytes __attribute__((unused));
    FifoReadPointer(fifo, &unaligned, &numberOfBytes); // FifoReadPacket will call FifoReadPointerComplete when data is read
    uint16_t packetSize;
    memcpy(&packetSize, (void*) unaligned, sizeof (packetSize)); // uint16_t cannot be dereferenced from an odd address
    return packetSize;
}

/**
 * @brief Reads a packet from the FIFO. This function must only be used if all
 * data in the FIFO was written using FifoWritePacket. The packet will not be
 * read if the packet size is greater than the destination size.
 * @param fifo FIFO structure.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @return Size of the packet. 0 if the packet was not read.
 */
static inline __attribute__((always_inline)) size_t FifoReadPacket(Fifo * const fifo, void* const destination, const size_t destinationSize) {
    const size_t packetSize = FifoAvailableReadPacket(fifo);
    if (packetSize == 0) {
        return 0;
    }
    if (packetSize > destinationSize) {
        return 0;
    }
    FifoReadPointerComplete(fifo, sizeof (uint16_t));
    FifoRead(fifo, destination, packetSize);
    return packetSize;
}

/**
 * @brief Reads packets from the FIFO. This function must only be used if all
 * data in the FIFO was written using FifoWritePacket. No packets will be read
 * if the packet size is greater than the destination size.
 * @param fifo FIFO structure.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @return Number of bytes read. 0 if no packets were read.
 */
static inline __attribute__((always_inline)) size_t FifoReadPackets(Fifo * const fifo, void* const destination, const size_t destinationSize) {
    size_t numberOfBytes = 0;
    while (true) {
        const size_t packetSize = FifoReadPacket(fifo, &((uint8_t*) destination)[numberOfBytes], destinationSize - numberOfBytes);
        if (packetSize == 0) {
            break;
        }
        numberOfBytes += packetSize;
    }
    return numberOfBytes;
}

/**
 * @brief Returns the space available to write to the FIFO.
 * @param fifo FIFO structure.
 * @return Space available in the buffer.
 */
static inline __attribute__((always_inline)) size_t FifoAvailableWrite(Fifo * const fifo) {
    const size_t readIndex = fifo->readIndex; // avoid asynchronous hazard
    if (fifo->writeIndex < readIndex) {
        return FifoCapacity(fifo) - (fifo->dataSize - readIndex) - fifo->writeIndex;
    } else {
        return FifoCapacity(fifo) - (fifo->writeIndex - readIndex);
    }
}

/**
 * @brief Writes data to the FIFO. This function is private.
 * @param fifo FIFO structure.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @param writeIndex Write index.
 * @return Write index.
 */
static inline __attribute__((always_inline)) size_t FifoWriteInternal(Fifo * const fifo, const void* const data, const size_t numberOfBytes, const size_t writeIndex) {

    // Write data with no wraparound
    if ((writeIndex + numberOfBytes) < fifo->dataSize) {
        memcpy((void*) &fifo->data[writeIndex], data, numberOfBytes);
        return writeIndex + numberOfBytes;
    }

    // Write data with wraparound
    const size_t numberOfBytesBeforeWraparound = fifo->dataSize - writeIndex;
    memcpy((void*) &fifo->data[writeIndex], data, numberOfBytesBeforeWraparound);
    const size_t numberOfBytesAfterWraparound = numberOfBytes - numberOfBytesBeforeWraparound;
    memcpy((void*) fifo->data, &((uint8_t*) data)[numberOfBytesBeforeWraparound], numberOfBytesAfterWraparound);
    return numberOfBytesAfterWraparound;
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
    fifo->writeIndex = FifoWriteInternal(fifo, data, numberOfBytes, fifo->writeIndex);
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
 * @brief Returns the space available to write a packet to the FIFO.
 * @param fifo FIFO structure.
 * @return Space available for packet payload.
 */
static inline __attribute__((always_inline)) size_t FifoAvailableWritePacket(Fifo * const fifo) {
    const size_t available = FifoAvailableWrite(fifo);
    const size_t overhead = sizeof (uint16_t) + 1; // includes extra byte in event of wraparound
    if (available <= overhead) {
        return 0;
    }
    return available - overhead;
}

/**
 * @brief Writes a packet to the FIFO.
 * @param fifo FIFO structure.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @return Result.
 */
static inline __attribute__((always_inline)) FifoResult FifoWritePacket(Fifo * const fifo, const void* const data, const size_t numberOfBytes) {
    if (numberOfBytes == 0) {
        return FifoResultError;
    }
    if (numberOfBytes > UINT16_MAX) {
        return FifoResultError;
    }
    if (numberOfBytes > FifoAvailableWritePacket(fifo)) {
        return FifoResultError;
    }
    size_t writeIndex = fifo->writeIndex; // avoid asynchronous hazard
    if (writeIndex == fifo->dataSize - 1) {
        writeIndex = 0; // packetSize must not be written to wrapround
    }
    const uint16_t packetSize = (uint16_t) numberOfBytes;
    writeIndex = FifoWriteInternal(fifo, &packetSize, sizeof (packetSize), writeIndex);
    fifo->writeIndex = FifoWriteInternal(fifo, data, numberOfBytes, writeIndex);
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
