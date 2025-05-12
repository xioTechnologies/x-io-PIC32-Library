/**
 * @file UartDma.h
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "PeripheralBusClockFrequency.h"
#include "UartDma.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Timer peripheral clock.
 */
#if defined __PIC32MX__
#define TIMER_PERIPHERAL_CLOCK (PERIPHERAL_BUS_CLOCK_FREQUENCY)
#elif defined __PIC32MZ__
#define TIMER_PERIPHERAL_CLOCK (PERIPHERAL_BUS_CLOCK_3_FREQUENCY)
#else
#error "Unsupported device."
#endif

//------------------------------------------------------------------------------
// Variables

const UartDmaReadConditions uartDmaReadConditionsDefault = {
    .numberOfBytes = 1024,
    .terminationByte = -1,
    .timeout = 10,
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Calculates the timer reset value for a timeout.
 * @param timeout Timeout in milliseconds.
 * @return Timer reset value.
 */
uint32_t UartDmaCalculateTimerResetValue(const uint32_t timeout) {
    return UINT32_MAX - (timeout * (TIMER_PERIPHERAL_CLOCK / 1000));
}

//------------------------------------------------------------------------------
// End of file
