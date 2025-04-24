/**
 * @file Spi3DmaTX.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices. DMA used for TX only.
 */

#ifndef SPI3_DMA_TX_H
#define SPI3_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function declarations

void Spi3DmaTXInitialise(const SpiSettings * const settings);
void Spi3DmaTXDeinitialise(void);
void Spi3DmaTXTransfer(const GPIO_PIN csPin_, const void* const data, const size_t numberOfBytes, void (*transferComplete_)(void));
bool Spi3DmaTXTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
