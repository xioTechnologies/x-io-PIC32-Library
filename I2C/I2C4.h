/**
 * @file I2C4.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

#ifndef I2C4_H
#define I2C4_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const I2C i2c4;

//------------------------------------------------------------------------------
// Function declarations

void I2C4Initialise(const I2CClockFrequency clockFrequency);
void I2C4Deinitialise(void);
void I2C4Start(void);
void I2C4RepeatedStart(void);
void I2C4Stop(void);
bool I2C4Transmit(const uint8_t byte);
bool I2C4TransmitAddressRead(const uint8_t address);
bool I2C4TransmitAddressWrite(const uint8_t address);
uint8_t I2C4ReceiveAck(void);
uint8_t I2C4ReceiveNack(void);
bool I2C4LockThenTransfer(const I2CInstruction * const instructions_, const int numberOfInstructions_, void (*const transferComplete_) (void));
bool I2C4Lock(void);
void I2C4Unlock(void);
bool I2C4Locked(void);

#endif

//------------------------------------------------------------------------------
// End of file
