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
// Function prototypes

void Uart4DmaInitialise(const UartSettings * const uartSettings, const UartDmaReadConditions * const uartDmaReadConditions);
void Uart4DmaDisable();
void Uart4DmaSetCallbackFunction(void (*read)(const void* const data, const size_t numberOfBytes));
void Uart4DmaRead();
void Uart4DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart4DmaWriteInProgress();
bool Uart4DmaReceiveBufferOverrun();
bool Uart4DmaTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
