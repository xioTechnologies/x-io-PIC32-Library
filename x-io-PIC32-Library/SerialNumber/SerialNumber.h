/**
 * @file SerialNumber.h
 * @author Seb Madgwick
 * @brief Provides the serial number of PIC32MZ devices.
 */

#ifndef SERIAL_NUMBER_H
#define SERIAL_NUMBER_H

//------------------------------------------------------------------------------
// Includes

#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Serial number string size.
 */
#define SERIAL_NUMBER_STRING_SIZE (sizeof("XXXX-XXXX-XXXX-XXXX"))

//------------------------------------------------------------------------------
// Function declarations

uint64_t SerialNumberGetUint64();
void SerialNumberGetString(char* const destination, const size_t destinationSize);

#endif

//------------------------------------------------------------------------------
// End of file
