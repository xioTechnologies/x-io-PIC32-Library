/**
 * @file Uart1Dma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART1_DMA_H
#define UART1_DMA_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function prototypes

void Uart1DmaInitialise(const UartSettings * const uartSettings, const UartDmaReadConditions * const uartDmaReadConditions);
void Uart1DmaDisable();
void Uart1DmaSetCallbackFunction(void (*read)(const void* const data, const size_t numberOfBytes));
void Uart1DmaRead();
void Uart1DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart1DmaWriteInProgress();
bool Uart1DmaReceiveBufferOverrun();
bool Uart1DmaTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
