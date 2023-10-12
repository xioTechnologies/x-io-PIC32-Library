/**
 * @file Uart5Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART5_DMA_H
#define UART5_DMA_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart5DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart5DmaDeinitialise();
void Uart5DmaSetReadCallback(void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart5DmaRead();
void Uart5DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart5DmaIsWriteInProgress();
bool Uart5DmaHasReceiveBufferOverrun();
bool Uart5DmaIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
