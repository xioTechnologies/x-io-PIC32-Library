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
bool I2C5Transmit(const uint8_t byte);
bool I2C5TransmitAddressRead(const uint8_t address);
bool I2C5TransmitAddressWrite(const uint8_t address);
uint8_t I2C5ReceiveAck(void);
uint8_t I2C5ReceiveNack(void);
bool I2C5LockThenTransfer(const I2CInstruction * const instructions_, const int numberOfInstructions_, void (*const transferComplete_) (void));
bool I2C5Lock(void);
void I2C5Unlock(void);
bool I2C5Locked(void);

#endif

//------------------------------------------------------------------------------
// End of file
