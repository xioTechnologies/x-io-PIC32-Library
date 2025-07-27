/**
 * @file Spi6DmaTx.h
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices. DMA used for TX only.
 */

#ifndef SPI6_DMA_TX_H
#define SPI6_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const Spi spi6DmaTx;

//------------------------------------------------------------------------------
// Function declarations

void Spi6DmaTxInitialise(const SpiSettings * const settings);
void Spi6DmaTxDeinitialise(void);
void Spi6DmaTxTransfer(const GPIO_PIN csPin_, void* const data, const size_t numberOfBytes, void (*transferComplete_)(void));
bool Spi6DmaTxTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
