/**
 * @file Uart2Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART2_DMA_H
#define UART2_DMA_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart2DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart2DmaDisable();
void Uart2DmaSetReadCallback(void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart2DmaRead();
void Uart2DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart2DmaIsWriteInProgress();
bool Uart2DmaHasReceiveBufferOverrun();
bool Uart2DmaIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
