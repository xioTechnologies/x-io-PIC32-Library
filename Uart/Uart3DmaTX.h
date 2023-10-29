/**
 * @file Uart3DmaTX.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.  DMA used for TX only.
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

void Uart3DmaTXInitialise(const UartSettings * const settings);
void Uart3DmaTXDeinitialise(void);
size_t Uart3DmaTXGetReadAvailable(void);
size_t Uart3DmaTXRead(void* const destination, size_t numberOfBytes);
uint8_t Uart3DmaTXReadByte(void);
void Uart3DmaTXWrite(const void* const data, const size_t numberOfBytes);
bool Uart3DmaTXIsWriteInProgress(void);
void Uart3DmaTXClearReadBuffer(void);
bool Uart3DmaTXHasReceiveBufferOverrun(void);
bool Uart3DmaTXIsTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
