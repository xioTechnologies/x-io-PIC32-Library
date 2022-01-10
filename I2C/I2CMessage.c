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
 * @param message Message.
 */
void I2CMessageStart(I2CMessage * const message) {

    // Initialise I2C message
    message->index = 0;
    message->event[0] = I2CMessageEventEnd;
    message->messageComplete = NULL;

    // Add start event
    if (message->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    message->event[message->index] = I2CMessageEventStart;
    message->event[++message->index] = I2CMessageEventEnd;
}

/**
 * @brief Appends a repeated start event to the I2C message.
 * @param message Message.
 */
void I2CMessageRepeatedStart(I2CMessage * const message) {
    if (message->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    message->event[message->index] = I2CMessageEventRepeatedStart;
    message->event[++message->index] = I2CMessageEventEnd;
}

/**
 * @brief Appends a send event to the I2C message.
 * @param message Message.
 */
void I2CMessageSend(I2CMessage * const message, const uint8_t byte) {
    if (message->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    message->event[message->index] = I2CMessageEventSend;
    message->data[message->index] = byte;
    message->event[++message->index] = I2CMessageEventEnd;
}

/**
 * @brief Appends a receive event followed by an ACK or NACK event to the I2C
 * message.
 * @param message Message.
 */
void I2CMessageReceive(I2CMessage * const message, uint8_t * const destination, const bool ack) {
    // Add receive event
    if (message->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    message->event[message->index] = I2CMessageEventReceive;
    message->event[++message->index] = I2CMessageEventEnd;

    // Add ACK/NACK event
    if (message->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    message->event[message->index] = ack ? I2CMessageEventAck : I2CMessageEventNack;
    message->destination[message->index] = destination;
    message->event[++message->index] = I2CMessageEventEnd;
}

/**
 * @brief Appends a stop event to the I2C message.
 * @param Message complete callback function.  This callback function will be
 * called from within an interrupt once the message is complete.
 * @param message Message.
 */
void I2CMessageStop(I2CMessage * const message, void (*messageComplete)()) {

    // Add stop event
    if (message->index >= I2C_MESSAGE_MAX_LENGTH) {
        return;
    }
    message->event[message->index] = I2CMessageEventStop;
    message->event[++message->index] = I2CMessageEventEnd;

    // Set callback function
    message->messageComplete = messageComplete;
}

//------------------------------------------------------------------------------
// End of file
