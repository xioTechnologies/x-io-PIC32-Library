/**
 * @file Uart5.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART5_H
#define UART5_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart5Initialise(const UartSettings * const settings);
void Uart5Deinitialise(void);
size_t Uart5GetReadAvailable(void);
size_t Uart5Read(void* const destination, size_t numberOfBytes);
uint8_t Uart5ReadByte(void);
size_t Uart5GetWriteAvailable(void);
FifoResult Uart5Write(const void* const data, const size_t numberOfBytes);
FifoResult Uart5WriteByte(const uint8_t byte);
void Uart5ClearReadBuffer(void);
void Uart5ClearWriteBuffer(void);
bool Uart5HasReceiveBufferOverrun(void);
bool Uart5TransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
