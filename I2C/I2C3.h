/**
 * @file I2C3.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32.
 */

#ifndef I2C3_H
#define I2C3_H

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include "I2CClientAddress.h"
#include "I2CMessage.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void I2C3Initialise(const I2CClockFrequency clockFrequency);
void I2C3Deinitialise(void);
void I2C3Start(void);
void I2C3RepeatedStart(void);
void I2C3Stop(void);
bool I2C3Send(const uint8_t byte);
uint8_t I2C3Receive(const bool ack);
void I2C3BeginMessage(I2CMessage * const message);
bool I2C3MessageInProgress(void);

#endif

//------------------------------------------------------------------------------
// End of file
