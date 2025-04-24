/**
 * @file I2C5.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

#ifndef I2C5_H
#define I2C5_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const I2C i2c5;

//------------------------------------------------------------------------------
// Function declarations

void I2C5Initialise(const I2CClockFrequency clockFrequency);
void I2C5Deinitialise(void);
void I2C5Start(void);
void I2C5RepeatedStart(void);
void I2C5Stop(void);
bool I2C5Send(const uint8_t byte);
bool I2C5SendAddressRead(const uint8_t address);
bool I2C5SendAddressWrite(const uint8_t address);
uint8_t I2C5Receive(const bool ack);

#endif

//------------------------------------------------------------------------------
// End of file
