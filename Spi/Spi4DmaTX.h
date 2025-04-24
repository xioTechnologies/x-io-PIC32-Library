/**
 * @file Spi4DmaTX.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices. DMA used for TX only.
 */

#ifndef SPI4_DMA_TX_H
#define SPI4_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Function declarations

void Spi4DmaTXInitialise(const SpiSettings * const settings);
void Spi4DmaTXDeinitialise(void);
void Spi4DmaTXTransfer(const GPIO_PIN csPin_, const void* const data, const size_t numberOfBytes, void (*transferComplete_)(void));
bool Spi4DmaTXTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
