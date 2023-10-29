/**
 * @file SerialNumber.h
 * @author Seb Madgwick
 * @brief Provides the serial number of PIC32MZ devices.
 */

#ifndef SERIAL_NUMBER_H
#define SERIAL_NUMBER_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

uint64_t SerialNumberGetValue(void);
const char* SerialNumberGetString(void);

#endif

//------------------------------------------------------------------------------
// End of file
