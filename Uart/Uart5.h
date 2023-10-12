/**
 * @file Uart5.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART5_H
#define UART5_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart5Initialise(const UartSettings * const settings);
void Uart5Deinitialise();
size_t Uart5GetReadAvailable();
size_t Uart5Read(void* const destination, size_t numberOfBytes);
uint8_t Uart5ReadByte();
size_t Uart5GetWriteAvailable();
void Uart5Write(const void* const data, const size_t numberOfBytes);
void Uart5WriteByte(const uint8_t byte);
void Uart5ClearReadBuffer();
void Uart5ClearWriteBuffer();
bool Uart5HasReceiveBufferOverrun();
bool Uart5IsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
