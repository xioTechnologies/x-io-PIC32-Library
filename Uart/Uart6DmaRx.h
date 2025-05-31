/**
 * @file Uart6DmaRx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for RX only.
 */

#ifndef UART6_DMA_RX_H
#define UART6_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart6DmaRxInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart6DmaRxDeinitialise(void);
void Uart6DmaRxRead(void);
size_t Uart6DmaRxAvailableWrite(void);
FifoResult Uart6DmaRxWrite(const void* const data, const size_t numberOfBytes);
FifoResult Uart6DmaRxWriteByte(const uint8_t byte);
void Uart6DmaRxClearWriteBuffer(void);
bool Uart6DmaRxReceiveBufferOverrun(void);
bool Uart6DmaRxTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
