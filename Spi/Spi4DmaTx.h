/**
 * @file Spi4DmaTx.h
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
// Variable declarations

extern const Spi spi4DmaTx;

//------------------------------------------------------------------------------
// Function declarations

void Spi4DmaTxInitialise(const SpiSettings * const settings);
void Spi4DmaTxDeinitialise(void);
void Spi4DmaTxTransfer(const GPIO_PIN csPin_, volatile void* const data, const size_t numberOfBytes, void (*transferComplete_)(void));
bool Spi4DmaTxTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
