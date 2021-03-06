/**
 * @file Uart4.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART4_H
#define UART4_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart4Initialise(const UartSettings * const settings);
void Uart4Disable();
size_t Uart4GetReadAvailable();
size_t Uart4Read(void* const destination, size_t numberOfBytes);
uint8_t Uart4ReadByte();
size_t Uart4GetWriteAvailable();
void Uart4Write(const void* const data, const size_t numberOfBytes);
void Uart4WriteByte(const uint8_t byte);
void Uart4ClearReadBuffer();
void Uart4ClearWriteBuffer();
bool Uart4HasReceiveBufferOverrun();
bool Uart4IsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
