/**
 * @file Uart4DmaTX.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.  DMA used for TX only.
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

void Uart4DmaTXInitialise(const UartSettings * const settings);
void Uart4DmaTXDeinitialise();
size_t Uart4DmaTXGetReadAvailable();
size_t Uart4DmaTXRead(void* const destination, size_t numberOfBytes);
uint8_t Uart4DmaTXReadByte();
void Uart4DmaTXWrite(const void* const data, const size_t numberOfBytes);
bool Uart4DmaTXIsWriteInProgress();
void Uart4DmaTXClearReadBuffer();
bool Uart4DmaTXHasReceiveBufferOverrun();
bool Uart4DmaTXIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
