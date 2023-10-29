/**
 * @file I2CStartSequence.h
 * @author Seb Madgwick
 * @brief I2C start sequence with acknowledge polling and timeout.
 */

#ifndef I2C_START_SEQUENCE_H
#define I2C_START_SEQUENCE_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

bool I2CStartSequence(void (*i2cStart)(void), bool(*i2cSend)(const uint8_t byte), const uint8_t slaveAddress, const uint32_t timeout_);

#endif

//------------------------------------------------------------------------------
// End of file
