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
void Uart1Deinitialise(void);
size_t Uart1GetReadAvailable(void);
size_t Uart1Read(void* const destination, size_t numberOfBytes);
uint8_t Uart1ReadByte(void);
size_t Uart1GetWriteAvailable(void);
void Uart1Write(const void* const data, const size_t numberOfBytes);
void Uart1WriteByte(const uint8_t byte);
void Uart1ClearReadBuffer(void);
void Uart1ClearWriteBuffer(void);
bool Uart1HasReceiveBufferOverrun(void);
bool Uart1TransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
