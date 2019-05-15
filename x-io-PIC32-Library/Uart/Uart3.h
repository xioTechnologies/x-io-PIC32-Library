/**
 * @file Uart3.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART3_H
#define UART3_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function prototypes

void Uart3Initialise(const UartSettings * const uartSettings);
void Uart3Disable();
size_t Uart3GetReadAvailable();
size_t Uart3Read(void* const destination, size_t numberOfBytes);
uint8_t Uart3ReadByte();
size_t Uart3GetWriteAvailable();
void Uart3Write(const void* const data, const size_t numberOfBytes);
void Uart3WriteByte(const uint8_t byte);
void Uart3WriteString(const char* string);
void Uart3ClearReadBuffer();
void Uart3ClearWriteBuffer();
bool Uart3ReadBufferOverrun();
bool Uart3TransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
