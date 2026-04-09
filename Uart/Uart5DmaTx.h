/**
 * @file Uart5DmaTx.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for TX only.
 */

#ifndef UART5_DMA_TX_H
#define UART5_DMA_TX_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "Uart.h"

//------------------------------------------------------------------------------
// Function declarations

void Uart5DmaTxInitialise(const UartSettings * const settings);
void Uart5DmaTxDeinitialise(void);
size_t Uart5DmaTxAvailableRead(void);
size_t Uart5DmaTxRead(void* const destination, size_t numberOfBytes);
uint8_t Uart5DmaTxReadByte(void);
size_t Uart5DmaTxAvailableWrite(void);
FifoResult Uart5DmaTxWrite(const void* const data, const size_t numberOfBytes);
void Uart5DmaTxWriteTransfer(const void* const data, const size_t numberOfBytes, void (*const writeTransferComplete_) (void));
bool Uart5DmaTxWriteTransferInProgress(void);
void Uart5DmaTxClearReadBuffer(void);
void Uart5DmaTxClearWriteBuffer(void);
bool Uart5DmaTxReceiveBufferOverrun(void);
bool Uart5DmaTxTransmissionComplete(void);

#endif

//------------------------------------------------------------------------------
// End of file
