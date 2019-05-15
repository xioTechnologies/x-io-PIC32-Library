/**
 * @file I2CMessage.c
 * @author Seb Madgwick
 * @brief I2C driver for PIC32.
 */

//------------------------------------------------------------------------------
// Includes

#include "I2CMessage.h"
#include <stddef.h>

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the I2C message with a start event.
 * @param i2cMessage I2C message.
 */
void I2CMessageStart(I2CMessage * const i2cMessage) {

    // Initialise I2C message
    i2cMessage->index = 0;
    i2cMessage->event[0] = I2CMessageEventEnd;
    i2cMessage->messageComplete = NULL;

    // Add start event
    if (i2cMessage->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    i2cMessage->event[i2cMessage->index] = I2CMessageEventStart;
    i2cMessage->event[++i2cMessage->index] = I2CMessageEventEnd;
}

/**
 * @brief Appends a restart event to the I2C message.
 * @param i2cMessage I2C message.
 */
void I2CMessageRestart(I2CMessage * const i2cMessage) {
    if (i2cMessage->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    i2cMessage->event[i2cMessage->index] = I2CMessageEventRestart;
    i2cMessage->event[++i2cMessage->index] = I2CMessageEventEnd;
}

/**
 * @brief Appends a send event to the I2C message.
 * @param i2cMessage I2C message.
 */
void I2CMessageSend(I2CMessage * const i2cMessage, const uint8_t byte) {
    if (i2cMessage->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    i2cMessage->event[i2cMessage->index] = I2CMessageEventSend;
    i2cMessage->data[i2cMessage->index] = byte;
    i2cMessage->event[++i2cMessage->index] = I2CMessageEventEnd;
}

/**
 * @brief Appends a receive event followed by an ACK or NACK event to the I2C
 * message.
 * @param i2cMessage I2C message.
 */
void I2CMessageReceive(I2CMessage * const i2cMessage, uint8_t * const destination, const bool ack) {
    // Add receive event
    if (i2cMessage->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    i2cMessage->event[i2cMessage->index] = I2CMessageEventReceive;
    i2cMessage->event[++i2cMessage->index] = I2CMessageEventEnd;

    // Add ACK/NACK event
    if (i2cMessage->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    i2cMessage->event[i2cMessage->index] = ack ? I2CMessageEventAck : I2CMessageEventNack;
    i2cMessage->destination[i2cMessage->index] = destination;
    i2cMessage->event[++i2cMessage->index] = I2CMessageEventEnd;
}

/**
 * @brief Appends a stop event to the I2C message.
 * @param Message complete callback function.  This callback function will be
 * called from within an interrupt once the message is complete.
 * @param i2cMessage I2C message.
 */
void I2CMessageStop(I2CMessage * const i2cMessage, void (*messageComplete)()) {

    // Add stop event
    if (i2cMessage->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    i2cMessage->event[i2cMessage->index] = I2CMessageEventStop;
    i2cMessage->event[++i2cMessage->index] = I2CMessageEventEnd;

    // Set callback function
    i2cMessage->messageComplete = messageComplete;
}

//------------------------------------------------------------------------------
// End of file
