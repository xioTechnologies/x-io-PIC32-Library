/**
 * @file SerialNumber.c
 * @author Seb Madgwick
 * @brief Provides the serial number of PIC32MZ devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include <stdio.h>

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Returns the serial number.
 * @return Serial number.
 */
uint64_t SerialNumberGetValue(void) {
    return ((uint64_t) DEVSN1 << 32) | ((uint64_t) DEVSN0);
}

/**
 * @brief Returns the serial number string.
 * @return Serial number string.
 */
const char* SerialNumberGetString(void) {
    const uint64_t serialNumber = SerialNumberGetValue();
    static char string[sizeof ("XXXX-XXXX-XXXX-XXXX")];
    snprintf(string, sizeof (string), "%04X-%04X-%04X-%04X",
            (unsigned int) ((serialNumber >> 48) & 0xFFFF),
            (unsigned int) ((serialNumber >> 32) & 0xFFFF),
            (unsigned int) ((serialNumber >> 16) & 0xFFFF),
            (unsigned int) (serialNumber & 0xFFFF));
    return string;
}

//------------------------------------------------------------------------------
// End of file
