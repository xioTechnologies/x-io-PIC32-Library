/**
 * @file Uart3Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART3_DMA_H
#define UART3_DMA_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart3DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart3DmaDeinitialise(void);
void Uart3DmaSetReadCallback(void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart3DmaRead(void);
void Uart3DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart3DmaWriteInProgress(void);
bool Uart3DmaHasReceiveBufferOverrun(void);
bool Uart3DmaTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
