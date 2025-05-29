/**
 * @file Uart2.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART2_H
#define UART2_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart2Initialise(const UartSettings * const settings);
void Uart2Deinitialise(void);
size_t Uart2AvailableRead(void);
size_t Uart2Read(void* const destination, size_t numberOfBytes);
uint8_t Uart2ReadByte(void);
size_t Uart2AvailableWrite(void);
FifoResult Uart2Write(const void* const data, const size_t numberOfBytes);
FifoResult Uart2WriteByte(const uint8_t byte);
void Uart2ClearReadBuffer(void);
void Uart2ClearWriteBuffer(void);
bool Uart2ReceiveBufferOverrun(void);
bool Uart2TransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
