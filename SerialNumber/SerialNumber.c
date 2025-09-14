/**
 * @file SerialNumber.c
 * @author Seb Madgwick
 * @brief Provides the 64-bit serial number of a PIC32MZ device.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Returns the serial number.
 * @return Serial number.
 */
uint64_t SerialNumberU64(void) {
    return ((uint64_t) DEVSN1 << 32) | ((uint64_t) DEVSN0);
}

/**
 * @brief Returns the serial number as a string.
 * @return Serial number as a string.
 */
const char* SerialNumberString(void) {
    const uint64_t serialNumber = SerialNumberU64();
    static char string[sizeof ("XXXX-XXXX-XXXX-XXXX")];
    if (strlen(string) == 0) {
        snprintf(string, sizeof (string), "%04X-%04X-%04X-%04X",
                (unsigned int) ((serialNumber >> 48) & 0xFFFF),
                (unsigned int) ((serialNumber >> 32) & 0xFFFF),
                (unsigned int) ((serialNumber >> 16) & 0xFFFF),
                (unsigned int) (serialNumber & 0xFFFF));
    }
    return string;
}

//------------------------------------------------------------------------------
// End of file
