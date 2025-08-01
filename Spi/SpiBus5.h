/**
 * @file SpiBus5.h
 * @author Seb Madgwick
 * @brief SPI bus.
 */

#ifndef SPI_BUS_5_H
#define SPI_BUS_5_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "SpiBus.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const SpiBus spiBus5;

//------------------------------------------------------------------------------
// Function declarations

SpiBusClient * const SpiBus5AddClient(const GPIO_PIN csPin);
void SpiBus5Transfer(SpiBusClient * const client, volatile void* const data, const size_t numberOfBytes, void (*transferComplete)(void));
bool SpiBus5TransferInProgress(const SpiBusClient * const client);

#endif

//------------------------------------------------------------------------------
// End of file
