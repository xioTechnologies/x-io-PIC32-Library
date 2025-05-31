/**
 * @file Uart2DmaRx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for RX only.
 */

#ifndef UART2_DMA_RX_H
#define UART2_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart2DmaRxInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart2DmaRxDeinitialise(void);
void Uart2DmaRxRead(void);
size_t Uart2DmaRxAvailableWrite(void);
FifoResult Uart2DmaRxWrite(const void* const data, const size_t numberOfBytes);
FifoResult Uart2DmaRxWriteByte(const uint8_t byte);
void Uart2DmaRxClearWriteBuffer(void);
bool Uart2DmaRxReceiveBufferOverrun(void);
bool Uart2DmaRxTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
