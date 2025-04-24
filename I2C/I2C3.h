/**
 * @file I2C3.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

#ifndef I2C3_H
#define I2C3_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const I2C i2c3;

//------------------------------------------------------------------------------
// Function declarations

void I2C3Initialise(const I2CClockFrequency clockFrequency);
void I2C3Deinitialise(void);
void I2C3Start(void);
void I2C3RepeatedStart(void);
void I2C3Stop(void);
bool I2C3Send(const uint8_t byte);
bool I2C3SendAddressRead(const uint8_t address);
bool I2C3SendAddressWrite(const uint8_t address);
uint8_t I2C3Receive(const bool ack);

#endif

//------------------------------------------------------------------------------
// End of file
