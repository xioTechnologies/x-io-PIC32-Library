/**
 * @file Spi6DmaTX.h
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
// Function declarations

void Spi6DmaTXInitialise(const SpiSettings * const settings);
void Spi6DmaTXDeinitialise(void);
void Spi6DmaTXTransfer(const GPIO_PIN csPin_, const void* const data, const size_t numberOfBytes, void (*transferComplete_)(void));
bool Spi6DmaTXTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
