/**
 * @file Uart4Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART4_DMA_H
#define UART4_DMA_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart4DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart4DmaDeinitialise(void);
void Uart4DmaRead(void);
void Uart4DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart4DmaWriteInProgress(void);
bool Uart4DmaReceiveBufferOverrun(void);
bool Uart4DmaTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
