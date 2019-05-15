/**
 * @file I2C.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

#ifndef I2C_H
#define I2C_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief I2C clock frequency.
 */
typedef enum {
    I2CClockFrequency100kHz = 100000,
    I2CClockFrequency400kHz = 400000,
    I2CClockFrequency1MHz = 1000000,
} I2CClockFrequency;

/**
 * @brief I2C timeout in timer ticks.  Equal to 10 clock cycles.
 */
#define I2C_TIMEOUT (TIMER_TICKS_PER_SECOND / (I2CClockFrequency100kHz / 10))

//------------------------------------------------------------------------------
// Function prototypes

uint32_t I2CCalculateI2Cxbrg(const uint32_t fsk);

#endif

//------------------------------------------------------------------------------
// End of file
