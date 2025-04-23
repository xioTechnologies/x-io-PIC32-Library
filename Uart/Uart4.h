/**
 * @file Uart4.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART4_H
#define UART4_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart4Initialise(const UartSettings * const settings);
void Uart4Deinitialise(void);
size_t Uart4GetReadAvailable(void);
size_t Uart4Read(void* const destination, size_t numberOfBytes);
uint8_t Uart4ReadByte(void);
size_t Uart4GetWriteAvailable(void);
FifoResult Uart4Write(const void* const data, const size_t numberOfBytes);
FifoResult Uart4WriteByte(const uint8_t byte);
void Uart4ClearReadBuffer(void);
void Uart4ClearWriteBuffer(void);
bool Uart4HasReceiveBufferOverrun(void);
bool Uart4TransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
