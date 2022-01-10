/**
 * @file I2C4.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32.
 */

#ifndef I2C4_H
#define I2C4_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include "I2CMessage.h"
#include "I2CSlaveAddress.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void I2C4Initialise(const I2CClockFrequency clockFrequency);
void I2C4Disable();
void I2C4Start();
void I2C4RepeatedStart();
void I2C4Stop();
bool I2C4Send(const uint8_t byte);
uint8_t I2C4Receive(const bool ack);
void I2C4BeginMessage(I2CMessage * const message);
bool I2C4IsMessageInProgress();

#endif

//------------------------------------------------------------------------------
// End of file
