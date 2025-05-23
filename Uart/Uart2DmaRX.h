/**
 * @file Uart2DmaRX.h
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

void Uart2DmaRXInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart2DmaRXDeinitialise(void);
void Uart2DmaRXRead(void);
size_t Uart2DmaRXGetWriteAvailable(void);
FifoResult Uart2DmaRXWrite(const void* const data, const size_t numberOfBytes);
FifoResult Uart2DmaRXWriteByte(const uint8_t byte);
void Uart2DmaRXClearWriteBuffer(void);
bool Uart2DmaRXHasReceiveBufferOverrun(void);
bool Uart2DmaRXTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
