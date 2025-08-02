/**
 * @file Spi6.h
 * @author Seb Madgwick
 * @brief SPI driver for PIC32 devices.
 */

#ifndef SPI6_H
#define SPI6_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const Spi spi6;

//------------------------------------------------------------------------------
// Function declarations

void Spi6Initialise(const SpiSettings * const settings);
void Spi6Deinitialise(void);
void Spi6Transfer(const GPIO_PIN csPin_, volatile void* const data_, const size_t numberOfBytes_, void (*const transferComplete_) (void));
bool Spi6TransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
