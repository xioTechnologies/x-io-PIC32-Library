/**
 * @file I2C5.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32.
 */

#ifndef I2C5_H
#define I2C5_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include "I2CMessage.h"
#include "I2CClientAddress.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void I2C5Initialise(const I2CClockFrequency clockFrequency);
void I2C5Deinitialise(void);
void I2C5Start(void);
void I2C5RepeatedStart(void);
void I2C5Stop(void);
bool I2C5Send(const uint8_t byte);
uint8_t I2C5Receive(const bool ack);
void I2C5BeginMessage(I2CMessage * const message);
bool I2C5MessageInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
