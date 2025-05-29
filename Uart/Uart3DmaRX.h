/**
 * @file Uart3DmaRX.h
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

void Uart3DmaRXInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart3DmaRXDeinitialise(void);
void Uart3DmaRXRead(void);
size_t Uart3DmaRXAvailableWrite(void);
FifoResult Uart3DmaRXWrite(const void* const data, const size_t numberOfBytes);
FifoResult Uart3DmaRXWriteByte(const uint8_t byte);
void Uart3DmaRXClearWriteBuffer(void);
bool Uart3DmaRXReceiveBufferOverrun(void);
bool Uart3DmaRXTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
