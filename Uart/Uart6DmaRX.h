/**
 * @file Uart6DmaRX.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.  DMA used for RX only.
 */

#ifndef UART6_DMA_RX_H
#define UART6_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart6DmaRXInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart6DmaRXDisable();
void Uart6DmaRXSetReadCallback(void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart6DmaRXRead();
size_t Uart6DmaRXGetWriteAvailable();
void Uart6DmaRXWrite(const void* const data, const size_t numberOfBytes);
void Uart6DmaRXWriteByte(const uint8_t byte);
void Uart6DmaRXClearWriteBuffer();
bool Uart6DmaRXHasReceiveBufferOverrun();
bool Uart6DmaRXIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
