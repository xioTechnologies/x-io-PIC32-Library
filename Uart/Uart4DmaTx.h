/**
 * @file Uart4DmaTx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for TX only.
 */

#ifndef UART4_DMA_TX_H
#define UART4_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart4DmaTxInitialise(const UartSettings * const settings);
void Uart4DmaTxDeinitialise(void);
size_t Uart4DmaTxAvailableRead(void);
size_t Uart4DmaTxRead(void* const destination, size_t numberOfBytes);
uint8_t Uart4DmaTxReadByte(void);
void Uart4DmaTxWrite(const void* const data, const size_t numberOfBytes, void (*writeComplete_)(void));
bool Uart4DmaTxWriteInProgress(void);
void Uart4DmaTxClearReadBuffer(void);
bool Uart4DmaTxReceiveBufferOverrun(void);
bool Uart4DmaTxTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
