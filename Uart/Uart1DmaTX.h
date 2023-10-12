/**
 * @file Uart1DmaTX.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.  DMA used for TX only.
 */

#ifndef UART1_DMA_TX_H
#define UART1_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart1DmaTXInitialise(const UartSettings * const settings);
void Uart1DmaTXDeinitialise();
size_t Uart1DmaTXGetReadAvailable();
size_t Uart1DmaTXRead(void* const destination, size_t numberOfBytes);
uint8_t Uart1DmaTXReadByte();
void Uart1DmaTXWrite(const void* const data, const size_t numberOfBytes);
bool Uart1DmaTXIsWriteInProgress();
void Uart1DmaTXClearReadBuffer();
bool Uart1DmaTXHasReceiveBufferOverrun();
bool Uart1DmaTXIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
