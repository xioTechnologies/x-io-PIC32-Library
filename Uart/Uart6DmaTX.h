/**
 * @file Uart6DmaTX.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.  DMA used for TX only.
 */

#ifndef UART6_DMA_TX_H
#define UART6_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart6DmaTXInitialise(const UartSettings * const settings);
void Uart6DmaTXDeinitialise(void);
size_t Uart6DmaTXGetReadAvailable(void);
size_t Uart6DmaTXRead(void* const destination, size_t numberOfBytes);
uint8_t Uart6DmaTXReadByte(void);
void Uart6DmaTXWrite(const void* const data, const size_t numberOfBytes);
bool Uart6DmaTXWriteInProgress(void);
void Uart6DmaTXClearReadBuffer(void);
bool Uart6DmaTXHasReceiveBufferOverrun(void);
bool Uart6DmaTXTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
