/**
 * @file Uart2DmaRX.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.  DMA used for RX only.
 */

#ifndef UART2_DMA_RX_H
#define UART2_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart2DmaRXInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart2DmaRXDisable();
void Uart2DmaRXSetReadCallback(void (*read)(const void* const data, const size_t numberOfBytes));
void Uart2DmaRXRead();
size_t Uart2DmaRXGetWriteAvailable();
void Uart2DmaRXWrite(const void* const data, const size_t numberOfBytes);
void Uart2DmaRXWriteByte(const uint8_t byte);
void Uart2DmaRXClearWriteBuffer();
bool Uart2DmaRXHasReceiveBufferOverrun();
bool Uart2DmaRXIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
