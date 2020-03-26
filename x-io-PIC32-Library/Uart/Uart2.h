/**
 * @file Uart2.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART2_H
#define UART2_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart2Initialise(const UartSettings * const settings);
void Uart2Disable();
size_t Uart2GetReadAvailable();
size_t Uart2Read(void* const destination, size_t numberOfBytes);
uint8_t Uart2ReadByte();
size_t Uart2GetWriteAvailable();
void Uart2Write(const void* const data, const size_t numberOfBytes);
void Uart2WriteByte(const uint8_t byte);
void Uart2WriteString(const char* string);
void Uart2ClearReadBuffer();
void Uart2ClearWriteBuffer();
bool Uart2HasReceiveBufferOverrun();
bool Uart2IsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
