/**
 * @file Uart1DmaRX.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.  DMA used for RX only.
 */

#ifndef UART1_DMA_RX_H
#define UART1_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart1DmaRXInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart1DmaRXDeinitialise(void);
void Uart1DmaRXSetReadCallback(void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart1DmaRXRead(void);
size_t Uart1DmaRXGetWriteAvailable(void);
void Uart1DmaRXWrite(const void* const data, const size_t numberOfBytes);
void Uart1DmaRXWriteByte(const uint8_t byte);
void Uart1DmaRXClearWriteBuffer(void);
bool Uart1DmaRXHasReceiveBufferOverrun(void);
bool Uart1DmaRXTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
