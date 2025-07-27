/**
 * @file Spi5Dma.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

#ifndef SPI5_DMA_H
#define SPI5_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const Spi spi5Dma;

//------------------------------------------------------------------------------
// Function declarations

void Spi5DmaInitialise(const SpiSettings * const settings);
void Spi5DmaDeinitialise(void);
void Spi5DmaTransfer(const GPIO_PIN csPin_, void* const data_, const size_t numberOfBytes_, void (*transferComplete_)(void));
bool Spi5DmaTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
