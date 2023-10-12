/**
 * @file I2C2.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32.
 */

#ifndef I2C2_H
#define I2C2_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include "I2CMessage.h"
#include "I2CSlaveAddress.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void I2C2Initialise(const I2CClockFrequency clockFrequency);
void I2C2Deinitialise();
void I2C2Start();
void I2C2RepeatedStart();
void I2C2Stop();
bool I2C2Send(const uint8_t byte);
uint8_t I2C2Receive(const bool ack);
void I2C2BeginMessage(I2CMessage * const message);
bool I2C2IsMessageInProgress();

#endif

//------------------------------------------------------------------------------
// End of file
