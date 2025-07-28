/**
 * @file Uart3DmaRx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for RX only.
 */

#ifndef UART3_DMA_RX_H
#define UART3_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart3DmaRxInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart3DmaRxDeinitialise(void);
void Uart3DmaRxRead(void);
size_t Uart3DmaRxAvailableWrite(void);
FifoResult Uart3DmaRxWrite(const void* const data, const size_t numberOfBytes);
FifoResult Uart3DmaRxWriteByte(const uint8_t byte);
void Uart3DmaRxClearWriteBuffer(void);
bool Uart3DmaRxReceiveBufferOverrun(void);
bool Uart3DmaRxTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
