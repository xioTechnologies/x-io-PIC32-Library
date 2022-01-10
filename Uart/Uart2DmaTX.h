/**
 * @file Uart2DmaTX.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.  DMA used for TX only.
 */

#ifndef UART2_DMA_TX_H
#define UART2_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart2DmaTXInitialise(const UartSettings * const settings);
void Uart2DmaTXDisable();
size_t Uart2DmaTXGetReadAvailable();
size_t Uart2DmaTXRead(void* const destination, size_t numberOfBytes);
uint8_t Uart2DmaTXReadByte();
void Uart2DmaTXWrite(const void* const data, const size_t numberOfBytes);
bool Uart2DmaTXIsWriteInProgress();
void Uart2DmaTXClearReadBuffer();
bool Uart2DmaTXHasReceiveBufferOverrun();
bool Uart2DmaTXIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
