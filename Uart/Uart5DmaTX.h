/**
 * @file Uart5DmaTx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for TX only.
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

void Uart5DmaTxInitialise(const UartSettings * const settings);
void Uart5DmaTxDeinitialise(void);
size_t Uart5DmaTxAvailableRead(void);
size_t Uart5DmaTxRead(void* const destination, size_t numberOfBytes);
uint8_t Uart5DmaTxReadByte(void);
void Uart5DmaTxWrite(const void* const data, const size_t numberOfBytes, void (*writeComplete_)(void));
bool Uart5DmaTxWriteInProgress(void);
void Uart5DmaTxClearReadBuffer(void);
bool Uart5DmaTxReceiveBufferOverrun(void);
bool Uart5DmaTxTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
