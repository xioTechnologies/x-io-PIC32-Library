/**
 * @file Uart4Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART4_DMA_H
#define UART4_DMA_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart4DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*const read_) (const void* const data, const size_t numberOfBytes));
void Uart4DmaDeinitialise(void);
void Uart4DmaTasks(void);
size_t Uart4DmaAvailableWrite(void);
FifoResult Uart4DmaWrite(const void* const data, const size_t numberOfBytes);
void Uart4DmaWriteTransfer(const void* const data, const size_t numberOfBytes, void (*const writeTransferComplete_) (void));
bool Uart4DmaWriteTransferInProgress(void);
void Uart4DmaClearWriteBuffer(void);
bool Uart4DmaReceiveBufferOverrun(void);
bool Uart4DmaTransmissionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
