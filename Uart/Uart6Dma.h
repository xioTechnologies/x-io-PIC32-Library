/**
 * @file Uart6Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART6_DMA_H
#define UART6_DMA_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart6DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart6DmaDeinitialise(void);
void Uart6DmaRead(void);
void Uart6DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart6DmaWriteInProgress(void);
bool Uart6DmaHasReceiveBufferOverrun(void);
bool Uart6DmaTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
