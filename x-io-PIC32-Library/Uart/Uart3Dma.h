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
// Function prototypes

void Uart3DmaInitialise(const UartSettings * const uartSettings, const UartDmaReadConditions * const uartDmaReadConditions);
void Uart3DmaDisable();
void Uart3DmaSetCallbackFunction(void (*read)(const void* const data, const size_t numberOfBytes));
void Uart3DmaRead();
void Uart3DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart3DmaWriteInProgress();
bool Uart3DmaReceiveBufferOverrun();
bool Uart3DmaTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
