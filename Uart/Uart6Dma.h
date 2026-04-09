/**
 * @file Uart6Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART6_DMA_H
#define UART6_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart6DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*const read_) (const void* const data, const size_t numberOfBytes));
void Uart6DmaDeinitialise(void);
void Uart6DmaTasks(void);
size_t Uart6DmaAvailableWrite(void);
FifoResult Uart6DmaWrite(const void* const data, const size_t numberOfBytes);
void Uart6DmaWriteTransfer(const void* const data, const size_t numberOfBytes, void (*const writeTransferComplete_) (void));
bool Uart6DmaWriteTransferInProgress(void);
void Uart6DmaClearWriteBuffer(void);
bool Uart6DmaReceiveBufferOverrun(void);
bool Uart6DmaTransmissionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
