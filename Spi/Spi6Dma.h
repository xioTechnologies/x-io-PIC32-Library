/**
 * @file Spi6Dma.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

#ifndef SPI6_DMA_H
#define SPI6_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function declarations

void Spi6DmaInitialise(const SpiSettings * const settings);
void Spi6DmaDisable();
void Spi6DmaSetTransferCompleteCallback(void (*transferComplete_)());
void Spi6DmaTransfer(void* const data, const size_t numberOfBytes);
bool Spi6DmaIsTransferInProgress();

#endif

//------------------------------------------------------------------------------
// End of file
