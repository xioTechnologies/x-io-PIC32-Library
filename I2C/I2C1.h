/**
 * @file I2C1.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

#ifndef I2C1_H
#define I2C1_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const I2C i2c1;

//------------------------------------------------------------------------------
// Function declarations

void I2C1Initialise(const I2CClockFrequency clockFrequency);
void I2C1Deinitialise(void);
void I2C1Start(void);
void I2C1RepeatedStart(void);
void I2C1Stop(void);
bool I2C1Send(const uint8_t byte);
bool I2C1SendAddressRead(const uint8_t address);
bool I2C1SendAddressWrite(const uint8_t address);
uint8_t I2C1Receive(const bool ack);

#endif

//------------------------------------------------------------------------------
// End of file
