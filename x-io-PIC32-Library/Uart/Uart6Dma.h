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
// Function prototypes

void Uart6DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart6DmaDisable();
void Uart6DmaSetCallbackFunction(void (*read)(const void* const data, const size_t numberOfBytes));
void Uart6DmaRead();
void Uart6DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart6DmaWriteInProgress();
bool Uart6DmaReceiveBufferOverrun();
bool Uart6DmaTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
