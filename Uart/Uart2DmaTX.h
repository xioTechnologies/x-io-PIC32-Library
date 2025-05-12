/**
 * @file Uart2DmaTX.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for TX only.
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
void Uart2DmaTXDeinitialise(void);
size_t Uart2DmaTXGetReadAvailable(void);
size_t Uart2DmaTXRead(void* const destination, size_t numberOfBytes);
uint8_t Uart2DmaTXReadByte(void);
void Uart2DmaTXWrite(const void* const data, const size_t numberOfBytes, void (*writeComplete_)(void));
bool Uart2DmaTXWriteInProgress(void);
void Uart2DmaTXClearReadBuffer(void);
bool Uart2DmaTXHasReceiveBufferOverrun(void);
bool Uart2DmaTXTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
