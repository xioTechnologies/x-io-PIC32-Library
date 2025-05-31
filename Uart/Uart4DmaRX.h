/**
 * @file Uart4DmaRx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for RX only.
 */

#ifndef UART4_DMA_RX_H
#define UART4_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart4DmaRxInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart4DmaRxDeinitialise(void);
void Uart4DmaRxRead(void);
size_t Uart4DmaRxAvailableWrite(void);
FifoResult Uart4DmaRxWrite(const void* const data, const size_t numberOfBytes);
FifoResult Uart4DmaRxWriteByte(const uint8_t byte);
void Uart4DmaRxClearWriteBuffer(void);
bool Uart4DmaRxReceiveBufferOverrun(void);
bool Uart4DmaRxTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
