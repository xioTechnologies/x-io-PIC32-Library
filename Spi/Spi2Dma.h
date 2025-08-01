/**
 * @file Spi2Dma.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

#ifndef SPI2_DMA_H
#define SPI2_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const Spi spi2Dma;

//------------------------------------------------------------------------------
// Function declarations

void Spi2DmaInitialise(const SpiSettings * const settings);
void Spi2DmaDeinitialise(void);
void Spi2DmaTransfer(const GPIO_PIN csPin_, volatile void* const data_, const size_t numberOfBytes_, void (*transferComplete_)(void));
bool Spi2DmaTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
