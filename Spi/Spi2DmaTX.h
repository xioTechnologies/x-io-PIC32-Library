/**
 * @file Spi2DmaTX.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices. DMA used for TX only.
 */

#ifndef SPI2_DMA_TX_H
#define SPI2_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function declarations

void Spi2DmaTXInitialise(const SpiSettings * const settings);
void Spi2DmaTXDeinitialise(void);
void Spi2DmaTXTransfer(const GPIO_PIN csPin_, void* const data_, const size_t numberOfBytes_, void (*transferComplete_)(void));
bool Spi2DmaTXTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
