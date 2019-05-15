/**
 * @file Spi5Dma.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

#ifndef SPI5_DMA_H
#define SPI5_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function prototypes

void Spi5DmaInitialise(const SpiSettings * const spiSettings);
void Spi5DmaDisable();
void Spi5DmaSetCallbackFunction(void (*transferComplete)());
void Spi5DmaTransfer(void* const data, const size_t numberOfBytes);
bool Spi5DmaTransferInProgress();

#endif

//------------------------------------------------------------------------------
// End of file
