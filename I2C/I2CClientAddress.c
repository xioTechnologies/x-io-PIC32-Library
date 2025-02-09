/**
 * @file I2CClientAddress.c
 * @author Seb Madgwick
 * @brief Functions for appending the R/W bit on a 7-bit I2C client address.
 */

//------------------------------------------------------------------------------
// Includes

#include "I2CClientAddress.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Appends the R/W bit on a 7-bit I2C client address to indicate a write.
 * @param address 7-bit client address.
 * @return 7-bit client address with appended R/W bit.
 */
uint8_t I2CClientAddressRead(const uint8_t address) {
    return (address << 1) | 1;
}

/**
 * @brief Appends the R/W bit on a 7-bit I2C client address to indicate a read.
 * @param address 7-bit client address.
 * @return 7-bit client address with appended R/W bit.
 */
uint8_t I2CClientAddressWrite(const uint8_t address) {
    return (address << 1) | 0;
}

//------------------------------------------------------------------------------
// End of file
