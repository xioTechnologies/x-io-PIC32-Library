/**
 * @file I2CBB2.h
 * @author Seb Madgwick
 * @brief I2C bit-bang driver.
 */

#ifndef I2CBB2_H
#define I2CBB2_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const I2C i2cBB2;

//------------------------------------------------------------------------------
// Function declarations

void I2CBB2BusClear(void);
void I2CBB2Start(void);
void I2CBB2RepeatedStart(void);
void I2CBB2Stop(void);
bool I2CBB2Send(const uint8_t byte);
uint8_t I2CBB2Receive(const bool ack);

#endif

//------------------------------------------------------------------------------
// End of file
