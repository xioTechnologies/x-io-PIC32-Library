/**
 * @file I2CMessage.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32.
 */

#ifndef I2C_MESSAGE_H
#define I2C_MESSAGE_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Maximum I2C message length.
 */
#define I2C_MESSAGE_MAX_LENGTH (128)

/**
 * @brief I2C message event.
 */
typedef enum {
    I2CMessageEventStart,
    I2CMessageEventRepeatedStart,
    I2CMessageEventStop,
    I2CMessageEventSend,
    I2CMessageEventReceive,
    I2CMessageEventAck,
    I2CMessageEventNack,
    I2CMessageEventEnd,
} I2CMessageEvent;

/**
 * @brief I2C message structure.  Structure members are used internally and must
 * not be accessed by the application.
 */
typedef struct {
    int index;
    I2CMessageEvent event[I2C_MESSAGE_MAX_LENGTH + 1];
    uint8_t data[I2C_MESSAGE_MAX_LENGTH];
    uint8_t* destination[I2C_MESSAGE_MAX_LENGTH];
    void (*complete)();
} I2CMessage;

//------------------------------------------------------------------------------
// Function declarations

void I2CMessageStart(I2CMessage * const message);
void I2CMessageRepeatedStart(I2CMessage * const message);
void I2CMessageSend(I2CMessage * const message, const uint8_t byte);
void I2CMessageReceive(I2CMessage * const message, uint8_t * const destination, const bool ack);
void I2CMessageStop(I2CMessage * const message, void (*complete)());

#endif

//------------------------------------------------------------------------------
// End of file
