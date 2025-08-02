/**
 * @file Spi1Dma.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

#ifndef SPI1_DMA_H
#define SPI1_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const Spi spi1Dma;

//------------------------------------------------------------------------------
// Function declarations

void Spi1DmaInitialise(const SpiSettings * const settings);
void Spi1DmaDeinitialise(void);
void Spi1DmaTransfer(const GPIO_PIN csPin_, volatile void* const data_, const size_t numberOfBytes_, void (*const transferComplete_) (void));
bool Spi1DmaTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
