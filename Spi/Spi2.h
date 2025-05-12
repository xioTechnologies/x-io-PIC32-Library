/**
 * @file Spi2.h
 * @author Seb Madgwick
 * @brief SPI driver for PIC32 devices.
 */

#ifndef SPI2_H
#define SPI2_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function declarations

void Spi2Initialise(const SpiSettings * const settings);
void Spi2Deinitialise(void);
void Spi2Transfer(const GPIO_PIN csPin_, void* const data_, const size_t numberOfBytes_, void (*transferComplete_)(void));
bool Spi2TransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
