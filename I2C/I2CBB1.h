/**
 * @file I2CBB1.h
 * @author Seb Madgwick
 * @brief I2C bit-bang driver.
 */

#ifndef I2CBB1_H
#define I2CBB1_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const I2C i2cBB1;

//------------------------------------------------------------------------------
// Function declarations

void I2CBB1BusClear(void);
void I2CBB1Start(void);
void I2CBB1RepeatedStart(void);
void I2CBB1Stop(void);
bool I2CBB1Send(const uint8_t byte);
bool I2CBB1SendAddressRead(const uint8_t address);
bool I2CBB1SendAddressWrite(const uint8_t address);
uint8_t I2CBB1Receive(const bool ack);

#endif

//------------------------------------------------------------------------------
// End of file
