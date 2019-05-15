/**
 * @file I2CSlaveAddress.h
 * @author Seb Madgwick
 * @brief Functions for appending the R/W bit on a 7-bit I2C slave address.
 */

#ifndef I2C_SLAVE_ADDRESS_H
#define I2C_SLAVE_ADDRESS_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function prototypes

uint8_t I2CSlaveAddressRead(const uint8_t slaveAddress);
uint8_t I2CSlaveAddressWrite(const uint8_t slaveAddress);

#endif

//------------------------------------------------------------------------------
// End of file
