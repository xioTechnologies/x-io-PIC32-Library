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
// Variable declarations

extern const Spi spi3Dma;

//------------------------------------------------------------------------------
// Function declarations

void Spi3DmaInitialise(const SpiSettings * const settings);
void Spi3DmaDeinitialise(void);
void Spi3DmaTransfer(const GPIO_PIN csPin_, volatile void* const data_, const size_t numberOfBytes_, void (*const transferComplete_) (void));
bool Spi3DmaTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
