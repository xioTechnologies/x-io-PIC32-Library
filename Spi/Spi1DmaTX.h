/**
 * @file Spi1DmaTX.h
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
// Function declarations

void Spi1DmaTXInitialise(const SpiSettings * const settings);
void Spi1DmaTXDeinitialise(void);
void Spi1DmaTXTransfer(const GPIO_PIN csPin_, void* const data_, const size_t numberOfBytes_, void (*transferComplete_)(void));
bool Spi1DmaTXTransferInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
