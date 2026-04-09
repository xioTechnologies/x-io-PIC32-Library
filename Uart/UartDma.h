/**
 * @file UartDma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

#ifndef UART_DMA_H
#define UART_DMA_H

//------------------------------------------------------------------------------
// Includes

#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Read conditions.
 */
typedef struct {
    size_t numberOfBytes;
    int termination; // 0 to 255 (-1 = disabled)
    uint32_t timeout; // milliseconds
} UartDmaReadConditions;

//------------------------------------------------------------------------------
// Variable declarations

extern const UartDmaReadConditions uartDmaReadConditionsDefault;

//------------------------------------------------------------------------------
// Function declarations

uint32_t UartDmaCalculateTimerReset(const uint32_t timeout);

#endif

//------------------------------------------------------------------------------
// End of file
