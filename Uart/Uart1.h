/**
 * @file Uart1.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART1_H
#define UART1_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart1Initialise(const UartSettings * const settings);
void Uart1Deinitialise(void);
size_t Uart1AvailableRead(void);
size_t Uart1Read(void* const destination, size_t numberOfBytes);
uint8_t Uart1ReadByte(void);
size_t Uart1AvailableWrite(void);
FifoResult Uart1Write(const void* const data, const size_t numberOfBytes);
FifoResult Uart1WriteByte(const uint8_t byte);
void Uart1ClearReadBuffer(void);
void Uart1ClearWriteBuffer(void);
bool Uart1ReceiveBufferOverrun(void);
bool Uart1TransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
