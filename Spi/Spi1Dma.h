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
// Function declarations

void Spi1DmaInitialise(const SpiSettings * const settings);
void Spi1DmaDeinitialise(void);
void Spi1DmaSetTransferCompleteCallback(void (*transferComplete_)(void));
void Spi1DmaTransfer(void* const data, const size_t numberOfBytes);
bool Spi1DmaIsTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
