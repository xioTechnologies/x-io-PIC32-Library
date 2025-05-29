/**
 * @file Uart6DmaRX.h
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

void Uart6DmaRXInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart6DmaRXDeinitialise(void);
void Uart6DmaRXRead(void);
size_t Uart6DmaRXAvailableWrite(void);
FifoResult Uart6DmaRXWrite(const void* const data, const size_t numberOfBytes);
FifoResult Uart6DmaRXWriteByte(const uint8_t byte);
void Uart6DmaRXClearWriteBuffer(void);
bool Uart6DmaRXReceiveBufferOverrun(void);
bool Uart6DmaRXTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
