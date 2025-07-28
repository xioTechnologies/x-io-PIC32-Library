/**
 * @file Uart1DmaRx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for RX only.
 */

#ifndef UART1_DMA_RX_H
#define UART1_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart1DmaRxInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart1DmaRxDeinitialise(void);
void Uart1DmaRxRead(void);
size_t Uart1DmaRxAvailableWrite(void);
FifoResult Uart1DmaRxWrite(const void* const data, const size_t numberOfBytes);
FifoResult Uart1DmaRxWriteByte(const uint8_t byte);
void Uart1DmaRxClearWriteBuffer(void);
bool Uart1DmaRxReceiveBufferOverrun(void);
bool Uart1DmaRxTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
