/**
 * @file I2C2.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

#ifndef I2C2_H
#define I2C2_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const I2C i2c2;

//------------------------------------------------------------------------------
// Function declarations

void I2C2Initialise(const I2CClockFrequency clockFrequency);
void I2C2Deinitialise(void);
void I2C2Start(void);
void I2C2RepeatedStart(void);
void I2C2Stop(void);
bool I2C2Transmit(const uint8_t byte);
bool I2C2TransmitAddressRead(const uint8_t address);
bool I2C2TransmitAddressWrite(const uint8_t address);
uint8_t I2C2ReceiveAck(void);
uint8_t I2C2ReceiveNack(void);
bool I2C2LockThenTransfer(const I2CInstruction * const instructions_, const int numberOfInstructions_, void (*const transferComplete_) (void));
bool I2C2Lock(void);
void I2C2Unlock(void);
bool I2C2Locked(void);

#endif

//------------------------------------------------------------------------------
// End of file
