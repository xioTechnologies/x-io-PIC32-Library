/**
 * @file Uart1DmaRX.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.  DMA used for RX only.
 */

#ifndef UART1_DMA_RX_H
#define UART1_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart1DmaRXInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart1DmaRXDeinitialise();
void Uart1DmaRXSetReadCallback(void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart1DmaRXRead();
size_t Uart1DmaRXGetWriteAvailable();
void Uart1DmaRXWrite(const void* const data, const size_t numberOfBytes);
void Uart1DmaRXWriteByte(const uint8_t byte);
void Uart1DmaRXClearWriteBuffer();
bool Uart1DmaRXHasReceiveBufferOverrun();
bool Uart1DmaRXIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
