/**
 * @file Spi4.h
 * @author Seb Madgwick
 * @brief SPI driver for PIC32 devices.
 */

#ifndef SPI4_H
#define SPI4_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const Spi spi4;

//------------------------------------------------------------------------------
// Function declarations

void Spi4Initialise(const SpiSettings * const settings);
void Spi4Deinitialise(void);
void Spi4Transfer(const GPIO_PIN csPin_, void* const data_, const size_t numberOfBytes_, void (*transferComplete_)(void));
bool Spi4TransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
