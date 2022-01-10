/**
 * @file Uart4DmaRX.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.  DMA used for RX only.
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
void Uart4DmaRXDisable();
void Uart4DmaRXSetReadCallback(void (*read)(const void* const data, const size_t numberOfBytes));
void Uart4DmaRXRead();
size_t Uart4DmaRXGetWriteAvailable();
void Uart4DmaRXWrite(const void* const data, const size_t numberOfBytes);
void Uart4DmaRXWriteByte(const uint8_t byte);
void Uart4DmaRXClearWriteBuffer();
bool Uart4DmaRXHasReceiveBufferOverrun();
bool Uart4DmaRXIsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
