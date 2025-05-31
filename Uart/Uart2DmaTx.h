/**
 * @file Uart2DmaTx.h
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

void Uart2DmaTxInitialise(const UartSettings * const settings);
void Uart2DmaTxDeinitialise(void);
size_t Uart2DmaTxAvailableRead(void);
size_t Uart2DmaTxRead(void* const destination, size_t numberOfBytes);
uint8_t Uart2DmaTxReadByte(void);
void Uart2DmaTxWrite(const void* const data, const size_t numberOfBytes, void (*writeComplete_)(void));
bool Uart2DmaTxWriteInProgress(void);
void Uart2DmaTxClearReadBuffer(void);
bool Uart2DmaTxReceiveBufferOverrun(void);
bool Uart2DmaTxTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
