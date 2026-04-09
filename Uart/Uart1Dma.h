/**
 * @file Uart1Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART1_DMA_H
#define UART1_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart1DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*const read_) (const void* const data, const size_t numberOfBytes));
void Uart1DmaDeinitialise(void);
void Uart1DmaTasks(void);
size_t Uart1DmaAvailableWrite(void);
FifoResult Uart1DmaWrite(const void* const data, const size_t numberOfBytes);
void Uart1DmaWriteTransfer(const void* const data, const size_t numberOfBytes, void (*const writeTransferComplete_) (void));
bool Uart1DmaWriteTransferInProgress(void);
void Uart1DmaClearWriteBuffer(void);
bool Uart1DmaReceiveBufferOverrun(void);
bool Uart1DmaTransmissionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
