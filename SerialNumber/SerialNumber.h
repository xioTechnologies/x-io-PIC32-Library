/**
 * @file SerialNumber.h
 * @author Seb Madgwick
 * @brief Provides the 64-bit serial number of a PIC32MZ device.
 */

#ifndef SERIAL_NUMBER_H
#define SERIAL_NUMBER_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

uint64_t SerialNumberU64(void);
const char* SerialNumberString(void);

#endif

//------------------------------------------------------------------------------
// End of file
