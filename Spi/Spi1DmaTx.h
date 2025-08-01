/**
 * @file Spi1DmaTx.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices. DMA used for TX only.
 */

#ifndef SPI1_DMA_TX_H
#define SPI1_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const Spi spi1DmaTx;

//------------------------------------------------------------------------------
// Function declarations

void Spi1DmaTxInitialise(const SpiSettings * const settings);
void Spi1DmaTxDeinitialise(void);
void Spi1DmaTxTransfer(const GPIO_PIN csPin_, volatile void* const data, const size_t numberOfBytes, void (*transferComplete_)(void));
bool Spi1DmaTxTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
