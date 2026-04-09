/**
 * @file Uart2Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART2_DMA_H
#define UART2_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart2DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*const read_) (const void* const data, const size_t numberOfBytes));
void Uart2DmaDeinitialise(void);
void Uart2DmaTasks(void);
size_t Uart2DmaAvailableWrite(void);
FifoResult Uart2DmaWrite(const void* const data, const size_t numberOfBytes);
void Uart2DmaWriteTransfer(const void* const data, const size_t numberOfBytes, void (*const writeTransferComplete_) (void));
bool Uart2DmaWriteTransferInProgress(void);
void Uart2DmaClearWriteBuffer(void);
bool Uart2DmaReceiveBufferOverrun(void);
bool Uart2DmaTransmissionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
