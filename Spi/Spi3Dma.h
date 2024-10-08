/**
 * @file Spi3Dma.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

#ifndef SPI3_DMA_H
#define SPI3_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function declarations

void Spi3DmaInitialise(const SpiSettings * const settings);
void Spi3DmaDeinitialise(void);
void Spi3DmaTransfer(const GPIO_PIN csPin_, void* const data_, const size_t numberOfBytes_, void (*transferComplete_)(void));
bool Spi3DmaTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
