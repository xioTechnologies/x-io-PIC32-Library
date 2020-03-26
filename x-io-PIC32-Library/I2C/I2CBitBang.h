/**
 * @file i2cBitBang.h
 * @author Seb Madgwick
 * @brief I2C bit-bang driver.
 */

#ifndef I2C_BIT_BANG_H
#define I2C_BIT_BANG_H

//------------------------------------------------------------------------------
// Includes

#include "I2CSlaveAddress.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void I2CBitBangBusClear();
void I2CBitBangStart();
void I2CBitBangStop();
bool I2CBitBangSend(const uint8_t byte);
uint8_t I2CBitBangReceive(const bool ack);

#endif

//------------------------------------------------------------------------------
// End of file
