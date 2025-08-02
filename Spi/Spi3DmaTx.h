/**
 * @file Spi3DmaTx.h
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
// Variable declarations

extern const Spi spi3DmaTx;

//------------------------------------------------------------------------------
// Function declarations

void Spi3DmaTxInitialise(const SpiSettings * const settings);
void Spi3DmaTxDeinitialise(void);
void Spi3DmaTxTransfer(const GPIO_PIN csPin_, volatile void* const data, const size_t numberOfBytes, void (*const transferComplete_) (void));
bool Spi3DmaTxTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
