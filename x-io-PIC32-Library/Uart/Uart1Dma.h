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
// Function declarations

void Uart1DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart1DmaDisable();
void Uart1DmaSetReadCallback(void (*read)(const void* const data, const size_t numberOfBytes));
void Uart1DmaRead();
void Uart1DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart1DmaIsWriteInProgress();
bool Uart1DmaHasReceiveBufferOverrun();
bool Uart1DmaIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
