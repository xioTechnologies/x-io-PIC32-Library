/**
 * @file I2CClientAddress.h
 * @author Seb Madgwick
 * @brief Functions for appending the R/W bit on a 7-bit I2C client address.
 */

#ifndef I2C_CLIENT_ADDRESS_H
#define I2C_CLIENT_ADDRESS_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

uint8_t I2CClientAddressRead(const uint8_t address);
uint8_t I2CClientAddressWrite(const uint8_t address);

#endif

//------------------------------------------------------------------------------
// End of file
