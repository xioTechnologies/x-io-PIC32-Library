/**
 * @file UartDma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART_DMA_H
#define UART_DMA_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief UART read conditions.
 */
typedef struct {
    uint32_t numberOfBytes;
    int terminatingByte; // -1 to disable
    uint32_t timeout; // milliseconds
} UartDmaReadConditions;

//------------------------------------------------------------------------------
// Variable declarations

extern const UartDmaReadConditions uartDmaReadConditionsDefault;

//------------------------------------------------------------------------------
// Function prototypes

uint32_t UartDmaCalculateTimerResetValue(const uint32_t timeout);

#endif

//------------------------------------------------------------------------------
// End of file
