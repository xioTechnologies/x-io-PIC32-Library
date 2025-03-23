/**
 * @file Uart3DmaRX.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for RX only.
 */

#ifndef UART3_DMA_RX_H
#define UART3_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart3DmaRXInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart3DmaRXDeinitialise(void);
void Uart3DmaRXSetReadCallback(void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart3DmaRXRead(void);
size_t Uart3DmaRXGetWriteAvailable(void);
void Uart3DmaRXWrite(const void* const data, const size_t numberOfBytes);
void Uart3DmaRXWriteByte(const uint8_t byte);
void Uart3DmaRXClearWriteBuffer(void);
bool Uart3DmaRXHasReceiveBufferOverrun(void);
bool Uart3DmaRXTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
