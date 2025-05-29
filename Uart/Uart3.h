/**
 * @file Uart3.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART3_H
#define UART3_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart3Initialise(const UartSettings * const settings);
void Uart3Deinitialise(void);
size_t Uart3AvailableRead(void);
size_t Uart3Read(void* const destination, size_t numberOfBytes);
uint8_t Uart3ReadByte(void);
size_t Uart3AvailableWrite(void);
FifoResult Uart3Write(const void* const data, const size_t numberOfBytes);
FifoResult Uart3WriteByte(const uint8_t byte);
void Uart3ClearReadBuffer(void);
void Uart3ClearWriteBuffer(void);
bool Uart3ReceiveBufferOverrun(void);
bool Uart3TransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
