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

void Uart1DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*const read_) (const void* const data, const size_t numberOfBytes));
void Uart1DmaDeinitialise(void);
void Uart1DmaRead(void);
void Uart1DmaWrite(const void* const data, const size_t numberOfBytes, void (*const writeComplete_) (void));
bool Uart1DmaWriteInProgress(void);
bool Uart1DmaReceiveBufferOverrun(void);
bool Uart1DmaTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
