/**
 * @file UartDma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "system_definitions.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Value used for UxBRG calculation.
 */
#if defined __PIC32MX__
#define TIMER_PERIPHERAL_CLOCK (SYS_CLK_BUS_PERIPHERAL_1)
#elif defined __PIC32MZ__
#define TIMER_PERIPHERAL_CLOCK (SYS_CLK_BUS_PERIPHERAL_3)
#else
#error "Unsupported device."
#endif

//------------------------------------------------------------------------------
// Variables

const UartDmaReadConditions uartDmaReadConditionsDefault = {
    .numberOfBytes = 1024,
    .terminatingByte = -1,
    .timeout = 10,
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Calculates a timer reset value for a specified timeout.
 * @param timeout Timeout in milliseconds.
 * @return Timer reset value.
 */
uint32_t UartDmaCalculateTimerResetValue(const uint32_t timeout) {
    return UINT32_MAX - (timeout * (TIMER_PERIPHERAL_CLOCK / 1000));
}

//------------------------------------------------------------------------------
// End of file
