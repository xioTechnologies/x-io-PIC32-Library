/**
 * @file Eeprom.h
 * @author Seb Madgwick
 * @brief Microchip 24-series I2C EEPROM driver.
 */

#ifndef EEPROM_H
#define EEPROM_H

//------------------------------------------------------------------------------
// Includes

#include "I2C/I2C.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Test result.
 */
typedef enum {
    EepromTestResultPassed,
    EepromTestResultAckFailed,
    EepromTestResultDataMismatch,
} EepromTestResult;

//------------------------------------------------------------------------------
// Function declarations

void EepromRead(const I2C * const i2c, const uint16_t address, void* const destination, const size_t numberOfBytes);
void EepromWrite(const I2C * const i2c, uint16_t address, const void* const data, const size_t numberOfBytes);
void EepromUpdate(const I2C * const i2c, uint16_t address, const void* const data, const size_t numberOfBytes);
void EepromErase(const I2C * const i2c);
bool EepromBlank(const I2C * const i2c);
void EepromPrint(const I2C * const i2c);
EepromTestResult EepromTest(const I2C * const i2c);
const char* EepromTestResultToString(const EepromTestResult result);

#endif

//------------------------------------------------------------------------------
// End of file
