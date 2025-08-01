/**
 * @file Spi5DmaTx.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices. DMA used for TX only.
 */

#ifndef SPI5_DMA_TX_H
#define SPI5_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const Spi spi5DmaTx;

//------------------------------------------------------------------------------
// Function declarations

void Spi5DmaTxInitialise(const SpiSettings * const settings);
void Spi5DmaTxDeinitialise(void);
void Spi5DmaTxTransfer(const GPIO_PIN csPin_, volatile void* const data, const size_t numberOfBytes, void (*transferComplete_)(void));
bool Spi5DmaTxTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
