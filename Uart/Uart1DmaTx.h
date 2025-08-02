/**
 * @file Uart1DmaTx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for TX only.
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

void Uart1DmaTxInitialise(const UartSettings * const settings);
void Uart1DmaTxDeinitialise(void);
size_t Uart1DmaTxAvailableRead(void);
size_t Uart1DmaTxRead(void* const destination, size_t numberOfBytes);
uint8_t Uart1DmaTxReadByte(void);
void Uart1DmaTxWrite(const void* const data, const size_t numberOfBytes, void (*const writeComplete_) (void));
bool Uart1DmaTxWriteInProgress(void);
void Uart1DmaTxClearReadBuffer(void);
bool Uart1DmaTxReceiveBufferOverrun(void);
bool Uart1DmaTxTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
