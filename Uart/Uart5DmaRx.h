/**
 * @file Uart5DmaRx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for RX only.
 */

#ifndef UART5_DMA_RX_H
#define UART5_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart5DmaRxInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart5DmaRxDeinitialise(void);
void Uart5DmaRxRead(void);
size_t Uart5DmaRxAvailableWrite(void);
FifoResult Uart5DmaRxWrite(const void* const data, const size_t numberOfBytes);
FifoResult Uart5DmaRxWriteByte(const uint8_t byte);
void Uart5DmaRxClearWriteBuffer(void);
bool Uart5DmaRxReceiveBufferOverrun(void);
bool Uart5DmaRxTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
