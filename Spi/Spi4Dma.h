/**
 * @file Spi4Dma.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

#ifndef SPI4_DMA_H
#define SPI4_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function declarations

void Spi4DmaInitialise(const SpiSettings * const settings);
void Spi4DmaDeinitialise(void);
void Spi4DmaTransfer(const GPIO_PIN csPin_, void* const data_, const size_t numberOfBytes_, void (*transferComplete_)(void));
bool Spi4DmaIsTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
