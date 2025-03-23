/**
 * @file Uart4DmaRX.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for RX only.
 */

#ifndef UART4_DMA_RX_H
#define UART4_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart4DmaRXInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart4DmaRXDeinitialise(void);
void Uart4DmaRXSetReadCallback(void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart4DmaRXRead(void);
size_t Uart4DmaRXGetWriteAvailable(void);
void Uart4DmaRXWrite(const void* const data, const size_t numberOfBytes);
void Uart4DmaRXWriteByte(const uint8_t byte);
void Uart4DmaRXClearWriteBuffer(void);
bool Uart4DmaRXHasReceiveBufferOverrun(void);
bool Uart4DmaRXTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
