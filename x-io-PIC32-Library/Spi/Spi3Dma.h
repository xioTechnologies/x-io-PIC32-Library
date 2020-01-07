/**
 * @file Spi3Dma.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

#ifndef SPI3_DMA_H
#define SPI3_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function prototypes

void Spi3DmaInitialise(const SpiSettings * const settings);
void Spi3DmaDisable();
void Spi3DmaSetTransferCompleteCallback(void (*transferComplete)());
void Spi3DmaTransfer(void* const data, const size_t numberOfBytes);
bool Spi3DmaIsTransferInProgress();

#endif

//------------------------------------------------------------------------------
// End of file
