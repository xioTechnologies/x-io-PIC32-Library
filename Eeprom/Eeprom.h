/**
 * @file Eeprom.h
 * @author Seb Madgwick
 * @brief Driver for Microchip 24xx32 to 24xx512 I2C EEPROM.
 */

#ifndef EEPROM_H
#define EEPROM_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void EepromRead(const uint16_t address, void* const destination, const size_t numberOfBytes);
void EepromWrite(uint16_t address, const void* const data, const size_t numberOfBytes);
void EepromUpdate(uint16_t address, const void* const data, const size_t numberOfBytes);
void EepromErase(void);
bool EepromBlank(void);
void EepromPrint(void);

#endif

//------------------------------------------------------------------------------
// End of file
