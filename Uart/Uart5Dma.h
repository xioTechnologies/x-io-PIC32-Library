/**
 * @file Uart5Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART5_DMA_H
#define UART5_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart5DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*const read_) (const void* const data, const size_t numberOfBytes));
void Uart5DmaDeinitialise(void);
void Uart5DmaTasks(void);
size_t Uart5DmaAvailableWrite(void);
FifoResult Uart5DmaWrite(const void* const data, const size_t numberOfBytes);
void Uart5DmaWriteTransfer(const void* const data, const size_t numberOfBytes, void (*const writeTransferComplete_) (void));
bool Uart5DmaWriteTransferInProgress(void);
void Uart5DmaClearWriteBuffer(void);
bool Uart5DmaReceiveBufferOverrun(void);
bool Uart5DmaTransmissionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
