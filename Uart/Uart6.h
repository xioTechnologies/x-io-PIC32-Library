/**
 * @file Uart6.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART6_H
#define UART6_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart6Initialise(const UartSettings * const settings);
void Uart6Deinitialise(void);
size_t Uart6GetReadAvailable(void);
size_t Uart6Read(void* const destination, size_t numberOfBytes);
uint8_t Uart6ReadByte(void);
size_t Uart6GetWriteAvailable(void);
void Uart6Write(const void* const data, const size_t numberOfBytes);
void Uart6WriteByte(const uint8_t byte);
void Uart6ClearReadBuffer(void);
void Uart6ClearWriteBuffer(void);
bool Uart6HasReceiveBufferOverrun(void);
bool Uart6TransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
