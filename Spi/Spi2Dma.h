/**
 * @file Spi2Dma.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

#ifndef SPI2_DMA_H
#define SPI2_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function declarations

void Spi2DmaInitialise(const SpiSettings * const settings);
void Spi2DmaDisable();
void Spi2DmaSetTransferCompleteCallback(void (*transferComplete)());
void Spi2DmaTransfer(void* const data, const size_t numberOfBytes);
bool Spi2DmaIsTransferInProgress();

#endif

//------------------------------------------------------------------------------
// End of file
