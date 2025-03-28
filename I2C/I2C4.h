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
#include "I2CClientAddress.h"
#include "I2CMessage.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void I2C4Initialise(const I2CClockFrequency clockFrequency);
void I2C4Deinitialise(void);
void I2C4Start(void);
void I2C4RepeatedStart(void);
void I2C4Stop(void);
bool I2C4Send(const uint8_t byte);
uint8_t I2C4Receive(const bool ack);
void I2C4BeginMessage(I2CMessage * const message_);
bool I2C4MessageInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
