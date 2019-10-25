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

void Uart4DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart4DmaDisable();
void Uart4DmaSetCallback(void (*read)(const void* const data, const size_t numberOfBytes));
void Uart4DmaRead();
void Uart4DmaWrite(const void* const data, const size_t numberOfBytes);
bool Uart4DmaIsWriteInProgress();
bool Uart4DmaHasReceiveBufferOverrun();
bool Uart4DmaIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
