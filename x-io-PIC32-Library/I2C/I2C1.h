/**
 * @file I2C1.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32.
 */

#ifndef I2C1_H
#define I2C1_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include "I2CMessage.h"
#include "I2CSlaveAddress.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Function prototypes

void I2C1Initialise(const I2CClockFrequency clockFrequency);
void I2C1Disable();
void I2C1Start();
void I2C1Restart();
void I2C1Stop();
bool I2C1Send(const uint8_t byte);
uint8_t I2C1Receive(const bool ack);
void I2C1BeginMessage(I2CMessage * const message);
bool I2C1IsMessageInProgress();

#endif

//------------------------------------------------------------------------------
// End of file
