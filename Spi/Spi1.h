/**
 * @file Spi1.h
 * @author Seb Madgwick
 * @brief SPI driver for PIC32 devices.
 */

#ifndef SPI1_H
#define SPI1_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const Spi spi1;

//------------------------------------------------------------------------------
// Function declarations

void Spi1Initialise(const SpiSettings * const settings);
void Spi1Deinitialise(void);
void Spi1Transfer(const GPIO_PIN csPin_, void* const data_, const size_t numberOfBytes_, void (*transferComplete_)(void));
bool Spi1TransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
