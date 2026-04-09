/**
 * @file Uart3Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART3_DMA_H
#define UART3_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart3DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*const read_) (const void* const data, const size_t numberOfBytes));
void Uart3DmaDeinitialise(void);
void Uart3DmaTasks(void);
size_t Uart3DmaAvailableWrite(void);
FifoResult Uart3DmaWrite(const void* const data, const size_t numberOfBytes);
void Uart3DmaWriteTransfer(const void* const data, const size_t numberOfBytes, void (*const writeTransferComplete_) (void));
bool Uart3DmaWriteTransferInProgress(void);
void Uart3DmaClearWriteBuffer(void);
bool Uart3DmaReceiveBufferOverrun(void);
bool Uart3DmaTransmissionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
