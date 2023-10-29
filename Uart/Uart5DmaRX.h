/**
 * @file Uart5DmaRX.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.  DMA used for RX only.
 */

#ifndef UART5_DMA_RX_H
#define UART5_DMA_RX_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "Uart.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart5DmaRXInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions);
void Uart5DmaRXDeinitialise(void);
void Uart5DmaRXSetReadCallback(void (*read_)(const void* const data, const size_t numberOfBytes));
void Uart5DmaRXRead(void);
size_t Uart5DmaRXGetWriteAvailable(void);
void Uart5DmaRXWrite(const void* const data, const size_t numberOfBytes);
void Uart5DmaRXWriteByte(const uint8_t byte);
void Uart5DmaRXClearWriteBuffer(void);
bool Uart5DmaRXHasReceiveBufferOverrun(void);
bool Uart5DmaRXIsTransmitionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
