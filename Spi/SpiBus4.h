/**
 * @file SpiBus4.h
 * @author Seb Madgwick
 * @brief SPI bus.
 */

#ifndef SPI_BUS_4_H
#define SPI_BUS_4_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "SpiBus.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const SpiBus spiBus4;

//------------------------------------------------------------------------------
// Function declarations

SpiBusClient * const SpiBus4AddClient(const GPIO_PIN csPin);
void SpiBus4Transfer(SpiBusClient * const client, void* const data, const size_t numberOfBytes, void (*transferComplete)(void));
bool SpiBus4TransferInProgress(const SpiBusClient * const client);

#endif

//------------------------------------------------------------------------------
// End of file
