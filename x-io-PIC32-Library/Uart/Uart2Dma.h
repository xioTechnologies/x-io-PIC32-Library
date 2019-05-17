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
// Function prototypes

void Uart2DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart2DmaDisable();
void Uart2DmaSetCallbackFunction(void (*read)(const void* const data, const size_t numberOfBytes));
void Uart2DmaRead();
void Uart2DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart2DmaWriteInProgress();
bool Uart2DmaReceiveBufferOverrun();
bool Uart2DmaTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
