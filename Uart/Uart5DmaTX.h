/**
 * @file Uart5DmaTX.h
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

void Uart5DmaTXInitialise(const UartSettings * const settings);
void Uart5DmaTXDeinitialise(void);
size_t Uart5DmaTXGetReadAvailable(void);
size_t Uart5DmaTXRead(void* const destination, size_t numberOfBytes);
uint8_t Uart5DmaTXReadByte(void);
void Uart5DmaTXWrite(const void* const data, const size_t numberOfBytes, void (*writeComplete_)(void));
bool Uart5DmaTXWriteInProgress(void);
void Uart5DmaTXClearReadBuffer(void);
bool Uart5DmaTXHasReceiveBufferOverrun(void);
bool Uart5DmaTXTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
