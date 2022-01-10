/**
 * @file I2C.c
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include "PeripheralBusClockFrequency.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Value used for UxBRG calculation.
 */
#if defined __PIC32MX__
#define PBCLK (PERIPHERAL_BUS_CLOCK_FREQUENCY)
#elif defined __PIC32MZ__
#define PBCLK (PERIPHERAL_BUS_CLOCK_2_FREQUENCY)
#else
#error "Unsupported device."
#endif

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Calculates a I2CXBRG value for a specified clock frequency.
 * See page 19 of Section 24. Inter-Integrated Circuit.
 * @param fsk Clock frequency in Hz.
 * @return I2CXBRG value.
 */
uint32_t I2CCalculateI2Cxbrg(const uint32_t fsk) {
    return (uint32_t) ((((1.0f / (2.0f * (float) fsk)) - 0.000000104f) * (float) PBCLK - 2.0f) + 0.5f);
}

//------------------------------------------------------------------------------
// End of file
