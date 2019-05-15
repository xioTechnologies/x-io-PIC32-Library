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
// Function prototypes

void Uart5DmaInitialise(const UartSettings * const uartSettings, const UartDmaReadConditions * const uartDmaReadConditions);
void Uart5DmaDisable();
void Uart5DmaSetCallbackFunction(void (*read)(const void* const data, const size_t numberOfBytes));
void Uart5DmaRead();
void Uart5DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart5DmaWriteInProgress();
bool Uart5DmaReceiveBufferOverrun();
bool Uart5DmaTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
