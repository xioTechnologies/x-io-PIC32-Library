/**
 * @file SpiBus.h
 * @author Seb Madgwick
 * @brief SPI bus.
 */

#ifndef SPI_BUS_H
#define SPI_BUS_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief SPI bus client. All structure members are private.
 */
typedef struct {
    GPIO_PIN csPin;
    volatile void* data;
    size_t numberOfBytes;
    void (*transferComplete)(void);
    volatile bool inProgress;
} SpiBusClient;

/**
 * @brief SPI bus interface.
 */
typedef struct {
    SpiBusClient * const (*addClient)(const GPIO_PIN csPin);
    void (*transfer)(SpiBusClient * const client, volatile void* const data, const size_t numberOfBytes, void (*transferComplete)(void));
    bool (*transferInProgress)(const SpiBusClient * const client);
} SpiBus;

#endif

//------------------------------------------------------------------------------
// End of file
