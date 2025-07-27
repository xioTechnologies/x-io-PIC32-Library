/**
 * @file Spi3.h
 * @author Seb Madgwick
 * @brief SPI driver for PIC32 devices.
 */

#ifndef SPI3_H
#define SPI3_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const Spi spi3;

//------------------------------------------------------------------------------
// Function declarations

void Spi3Initialise(const SpiSettings * const settings);
void Spi3Deinitialise(void);
void Spi3Transfer(const GPIO_PIN csPin_, void* const data_, const size_t numberOfBytes_, void (*transferComplete_)(void));
bool Spi3TransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
