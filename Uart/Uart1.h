/**
 * @file Uart1.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART1_H
#define UART1_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart1Initialise(const UartSettings * const settings);
void Uart1Disable();
size_t Uart1GetReadAvailable();
size_t Uart1Read(void* const destination, size_t numberOfBytes);
uint8_t Uart1ReadByte();
size_t Uart1GetWriteAvailable();
void Uart1Write(const void* const data, const size_t numberOfBytes);
void Uart1WriteByte(const uint8_t byte);
void Uart1ClearReadBuffer();
void Uart1ClearWriteBuffer();
bool Uart1HasReceiveBufferOverrun();
bool Uart1IsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
