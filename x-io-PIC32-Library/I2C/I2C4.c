/**
 * @file I2C4.c
 * @author Seb Madgwick
 * @brief I2C driver for PIC32.
 */

//------------------------------------------------------------------------------
// Includes

#include "I2C4.h"
#include <stddef.h>
#include "system_definitions.h"
#include "Timer/Timer.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Interrupt vector.
 */
#if defined _I2C_4_VECTOR
#define INTERRUPT_VECTOR _I2C_4_VECTOR
#else
#define INTERRUPT_VECTOR _I2C4_MASTER_VECTOR
#endif

//------------------------------------------------------------------------------
// Function prototypes

static void WaitForInterruptOrTimeout();

//------------------------------------------------------------------------------
// Variables

static I2CMessage* currentmessage;
static uint64_t messageTimeout;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param clockFrequency Clock frequency.
 */
void I2C4Initialise(const I2CClockFrequency clockFrequency) {

    // Ensure default register states
    I2C4Disable();

    // Configure I2C
    I2C4BRG = I2CCalculateI2Cxbrg(clockFrequency);
    if (clockFrequency != I2CClockFrequency400kHz) {
        I2C4CONbits.DISSLW = 1; // Slew rate control disabled
    }
    I2C4CONbits.I2CEN = 1; // Enables the I2C module and configures the SDA and SCL pins as serial port pins

    // Configure interrupt
    SYS_INT_VectorPrioritySet(INTERRUPT_VECTOR, INT_PRIORITY_LEVEL5);
}

/**
 * @brief Disable the module.
 */
void I2C4Disable() {

    // Disable I2C and restore default register states
    I2C4CON = 0;
    I2C4STAT = 0;

    // Disable interrupt
    SYS_INT_SourceDisable(INT_SOURCE_I2C_4_MASTER);
    SYS_INT_SourceStatusClear(INT_SOURCE_I2C_4_MASTER);
}

/**
 * @brief Performs the start condition.
 */
void I2C4Start() {
    SYS_INT_SourceStatusClear(INT_SOURCE_I2C_4_MASTER);
    I2C4CONbits.SEN = 1;
    WaitForInterruptOrTimeout();
}

/**
 * @brief Performs the restart condition.
 */
void I2C4Restart() {
    SYS_INT_SourceStatusClear(INT_SOURCE_I2C_4_MASTER);
    I2C4CONbits.RSEN = 1;
    WaitForInterruptOrTimeout();
}

/**
 * @brief Performs the stop condition.
 */
void I2C4Stop() {
    SYS_INT_SourceStatusClear(INT_SOURCE_I2C_4_MASTER);
    I2C4CONbits.PEN = 1;
    WaitForInterruptOrTimeout();
}

/**
 * @brief Sends a byte and checks for ACK.
 * @param byte Byte.
 * @return True if an ACK was generated.
 */
bool I2C4Send(const uint8_t byte) {
    SYS_INT_SourceStatusClear(INT_SOURCE_I2C_4_MASTER);
    I2C4STATbits.ACKSTAT = 0;
    I2C4TRN = byte;
    WaitForInterruptOrTimeout();
    return I2C4STATbits.ACKSTAT == 0;
}

/**
 * @brief Receives a byte and generates an ACK or NACK.
 * @param ack True for ACK.
 * @return Byte.
 */
uint8_t I2C4Receive(const bool ack) {

    // Receive
    SYS_INT_SourceStatusClear(INT_SOURCE_I2C_4_MASTER);
    I2C4CONbits.RCEN = 1;
    WaitForInterruptOrTimeout();

    // ACK/NACK
    SYS_INT_SourceStatusClear(INT_SOURCE_I2C_4_MASTER);
    I2C4CONbits.ACKDT = (ack == true) ? 0 : 1;
    I2C4CONbits.ACKEN = 1;
    WaitForInterruptOrTimeout();
    return I2C4RCV;
}

/**
 * @brief Waits for the interrupt or timeout.
 */
static void WaitForInterruptOrTimeout() {
    const uint64_t startTicks = TimerGetTicks64();
    const uint64_t timeout = TIMER_TICKS_PER_SECOND / (I2CClockFrequency100kHz / 10); // 10 clock cycles timeout for slowest clock
    while (true) {
        if (SYS_INT_SourceStatusGet(INT_SOURCE_I2C_4_MASTER) == true) {
            break;
        }
        if ((TimerGetTicks64() - startTicks) > timeout) {
            break;
        }
    }
}

/**
 * @brief Begins message.
 * @param message Message.
 */
void I2C4BeginMessage(I2CMessage * const message) {

    // Do nothing if message in progress
    if (I2C4MessageInProgress() == true) {
        return;
    }

    // Set current message
    currentmessage = message;
    message->index = 0;

    // Calculate message timeout
    messageTimeout = TimerGetTicks64() + (I2C_MESSAGE_MAX_LENGTH * I2C_TIMEOUT);

    // Trigger first interrupt
    SYS_INT_SourceStatusSet(INT_SOURCE_I2C_4_MASTER);
    SYS_INT_SourceEnable(INT_SOURCE_I2C_4_MASTER);
}

/**
 * @brief Returns true if the message is in progress.
 * @return True if the message is in progress.
 */
bool I2C4MessageInProgress() {
    if (TimerGetTicks64() >= messageTimeout) {
        SYS_INT_SourceDisable(INT_SOURCE_I2C_4_MASTER);
    }
    return SYS_INT_SourceIsEnabled(INT_SOURCE_I2C_4_MASTER);
}

/**
 * @brief I2C interrupt.
 */
void __ISR(INTERRUPT_VECTOR) I2C4Interrupt() {
    SYS_INT_SourceStatusClear(INT_SOURCE_I2C_4_MASTER); // clear interrupt flag first because next event may complete before ISR returns
    const int index = currentmessage->index++;
    switch (currentmessage->event[index]) {
        case I2CMessageEventStart:
            I2C4CONbits.SEN = 1;
            break;
        case I2CMessageEventRestart:
            I2C4CONbits.RSEN = 1;
            break;
        case I2CMessageEventStop:
            I2C4CONbits.PEN = 1;
            break;
        case I2CMessageEventSend:
            I2C4TRN = currentmessage->data[index];
            break;
        case I2CMessageEventReceive:
            I2C4CONbits.RCEN = 1;
            break;
        case I2CMessageEventAck:
            *currentmessage->destination[index] = I2C4RCV;
            I2C4CONbits.ACKDT = 0;
            I2C4CONbits.ACKEN = 1;
            break;
        case I2CMessageEventNack:
            *currentmessage->destination[index] = I2C4RCV;
            I2C4CONbits.ACKDT = 1;
            I2C4CONbits.ACKEN = 1;
            break;
        case I2CMessageEventEnd:
            SYS_INT_SourceDisable(INT_SOURCE_I2C_4_MASTER);
            if (currentmessage->messageComplete != NULL) {
                currentmessage->messageComplete();
            }
            break;
    }
}

//------------------------------------------------------------------------------
// End of file
