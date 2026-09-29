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
bool I2C3Transmit(const uint8_t byte);
bool I2C3TransmitAddressRead(const uint8_t address);
bool I2C3TransmitAddressWrite(const uint8_t address);
uint8_t I2C3ReceiveAck(void);
uint8_t I2C3ReceiveNack(void);
bool I2C3LockThenTransfer(const I2CInstruction * const instructions_, const int numberOfInstructions_, void (*const transferComplete_) (void));
bool I2C3Lock(void);
void I2C3Unlock(void);
bool I2C3Locked(void);

#endif

//------------------------------------------------------------------------------
// End of file
