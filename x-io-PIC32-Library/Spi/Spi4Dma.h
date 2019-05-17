/**
 * @file Spi4Dma.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

#ifndef SPI4_DMA_H
#define SPI4_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function prototypes

void Spi4DmaInitialise(const SpiSettings * const settings);
void Spi4DmaDisable();
void Spi4DmaSetCallbackFunction(void (*transferComplete)());
void Spi4DmaTransfer(void* const data, const size_t numberOfBytes);
bool Spi4DmaTransferInProgress();

#endif

//------------------------------------------------------------------------------
// End of file
