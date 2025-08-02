/**
 * @file Spi5.h
 * @author Seb Madgwick
 * @brief SPI driver for PIC32 devices.
 */

#ifndef SPI5_H
#define SPI5_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const Spi spi5;

//------------------------------------------------------------------------------
// Function declarations

void Spi5Initialise(const SpiSettings * const settings);
void Spi5Deinitialise(void);
void Spi5Transfer(const GPIO_PIN csPin_, volatile void* const data_, const size_t numberOfBytes_, void (*const transferComplete_) (void));
bool Spi5TransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
