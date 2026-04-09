/**
 * @file Uart6DmaTx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for TX only.
 */

#ifndef UART6_DMA_TX_H
#define UART6_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart6DmaTxInitialise(const UartSettings * const settings);
void Uart6DmaTxDeinitialise(void);
size_t Uart6DmaTxAvailableRead(void);
size_t Uart6DmaTxRead(void* const destination, size_t numberOfBytes);
uint8_t Uart6DmaTxReadByte(void);
size_t Uart6DmaTxAvailableWrite(void);
FifoResult Uart6DmaTxWrite(const void* const data, const size_t numberOfBytes);
void Uart6DmaTxWriteTransfer(const void* const data, const size_t numberOfBytes, void (*const writeTransferComplete_) (void));
bool Uart6DmaTxWriteTransferInProgress(void);
void Uart6DmaTxClearReadBuffer(void);
void Uart6DmaTxClearWriteBuffer(void);
bool Uart6DmaTxReceiveBufferOverrun(void);
bool Uart6DmaTxTransmissionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
