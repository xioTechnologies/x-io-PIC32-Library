/**
 * @file Spi2DmaTx.h
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

void Spi2DmaTxInitialise(const SpiSettings * const settings);
void Spi2DmaTxDeinitialise(void);
void Spi2DmaTxTransfer(const GPIO_PIN csPin_, const void* const data, const size_t numberOfBytes, void (*transferComplete_)(void));
bool Spi2DmaTxTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
