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
// Function declarations

void Uart3Initialise(const UartSettings * const settings);
void Uart3Deinitialise();
size_t Uart3GetReadAvailable();
size_t Uart3Read(void* const destination, size_t numberOfBytes);
uint8_t Uart3ReadByte();
size_t Uart3GetWriteAvailable();
void Uart3Write(const void* const data, const size_t numberOfBytes);
void Uart3WriteByte(const uint8_t byte);
void Uart3ClearReadBuffer();
void Uart3ClearWriteBuffer();
bool Uart3HasReceiveBufferOverrun();
bool Uart3IsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
