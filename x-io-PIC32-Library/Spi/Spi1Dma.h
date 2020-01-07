/**
 * @file Spi1Dma.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

#ifndef SPI1_DMA_H
#define SPI1_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function prototypes

void Spi1DmaInitialise(const SpiSettings * const settings);
void Spi1DmaDisable();
void Spi1DmaSetTransferCompleteCallback(void (*transferComplete)());
void Spi1DmaTransfer(void* const data, const size_t numberOfBytes);
bool Spi1DmaIsTransferInProgress();

#endif

//------------------------------------------------------------------------------
// End of file
