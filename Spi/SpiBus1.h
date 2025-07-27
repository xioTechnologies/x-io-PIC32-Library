/**
 * @file SpiBus1.h
 * @author Seb Madgwick
 * @brief SPI bus.
 */

#ifndef SPI_BUS_1_H
#define SPI_BUS_1_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "SpiBus.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const SpiBus spiBus1;

//------------------------------------------------------------------------------
// Function declarations

SpiBusClient * const SpiBus1AddClient(const GPIO_PIN csPin);
void SpiBus1Transfer(SpiBusClient * const client, void* const data, const size_t numberOfBytes, void (*transferComplete)(void));
bool SpiBus1TransferInProgress(const SpiBusClient * const client);

#endif

//------------------------------------------------------------------------------
// End of file
