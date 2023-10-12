/**
 * @file Uart5DmaTX.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.  DMA used for TX only.
 */

#ifndef UART5_DMA_TX_H
#define UART5_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart5DmaTXInitialise(const UartSettings * const settings);
void Uart5DmaTXDeinitialise();
size_t Uart5DmaTXGetReadAvailable();
size_t Uart5DmaTXRead(void* const destination, size_t numberOfBytes);
uint8_t Uart5DmaTXReadByte();
void Uart5DmaTXWrite(const void* const data, const size_t numberOfBytes);
bool Uart5DmaTXIsWriteInProgress();
void Uart5DmaTXClearReadBuffer();
bool Uart5DmaTXHasReceiveBufferOverrun();
bool Uart5DmaTXIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
