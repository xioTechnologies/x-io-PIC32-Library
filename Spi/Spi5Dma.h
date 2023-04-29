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
// Function declarations

void Spi5DmaInitialise(const SpiSettings * const settings);
void Spi5DmaDisable();
void Spi5DmaSetTransferCompleteCallback(void (*transferComplete_)());
void Spi5DmaTransfer(void* const data, const size_t numberOfBytes);
bool Spi5DmaIsTransferInProgress();

#endif

//------------------------------------------------------------------------------
// End of file
