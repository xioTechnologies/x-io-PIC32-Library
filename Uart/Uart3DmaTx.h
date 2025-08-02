/**
 * @file Uart3DmaTx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for TX only.
 */

#ifndef UART3_DMA_TX_H
#define UART3_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart3DmaTxInitialise(const UartSettings * const settings);
void Uart3DmaTxDeinitialise(void);
size_t Uart3DmaTxAvailableRead(void);
size_t Uart3DmaTxRead(void* const destination, size_t numberOfBytes);
uint8_t Uart3DmaTxReadByte(void);
void Uart3DmaTxWrite(const void* const data, const size_t numberOfBytes, void (*const writeComplete_) (void));
bool Uart3DmaTxWriteInProgress(void);
void Uart3DmaTxClearReadBuffer(void);
bool Uart3DmaTxReceiveBufferOverrun(void);
bool Uart3DmaTxTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
