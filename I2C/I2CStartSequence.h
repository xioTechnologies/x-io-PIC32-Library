/**
 * @file I2CStartSequence.h
 * @author Seb Madgwick
 * @brief I2C start sequence with acknowledge polling and timeout.
 */

#ifndef I2C_START_SEQUENCE_H
#define I2C_START_SEQUENCE_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

bool I2CStartSequence(const I2C * const i2c, const uint8_t address, const uint32_t timeout_);

#endif

//------------------------------------------------------------------------------
// End of file
