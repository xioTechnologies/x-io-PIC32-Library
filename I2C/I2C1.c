/**
 * @file I2C1.c
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "I2C1.h"
#include <stddef.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Uncomment this line to enable printing of messages.
 */
//#define PRINT_MESSAGES

//------------------------------------------------------------------------------
// Function declarations

static void WaitForInterruptOrTimeout(void);

//------------------------------------------------------------------------------
// Variables

const I2C i2c1 = {
    .start = I2C1Start,
    .repeatedStart = I2C1RepeatedStart,
    .stop = I2C1Stop,
    .send = I2C1Send,
    .sendAddressRead = I2C1SendAddressRead,
    .sendAddressWrite = I2C1SendAddressWrite,
    .receive = I2C1Receive,
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param clockFrequency Clock frequency.
 */
void I2C1Initialise(const I2CClockFrequency clockFrequency) {

    // Ensure default register states
    I2C1Deinitialise();

    // Configure I2C
    I2C1BRG = I2CCalculateI2Cxbrg(clockFrequency);
    if (clockFrequency != I2CClockFrequency400kHz) {
        I2C1CONbits.DISSLW = 1; // slew rate control disabled
    }
    I2C1CONbits.I2CEN = 1;
}

/**
 * @brief Disable the module.
 */
void I2C1Deinitialise(void) {

    // Disable I2C and restore default register states
    I2C1CON = 0;
    I2C1STAT = 0;

    // Disable interrupt
    EVIC_SourceDisable(INT_SOURCE_I2C1_MASTER);
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
}

/**
 * @brief Generates a start event.
 */
void I2C1Start(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1CONbits.SEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintStart();
#endif
}

/**
 * @brief Generates a repeated start event.
 */
void I2C1RepeatedStart(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1CONbits.RSEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintRepeatedStart();
#endif
}

/**
 * @brief Generates a stop event.
 */
void I2C1Stop(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1CONbits.PEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintStop();
#endif
}

/**
 * @brief Sends a byte and checks for ACK.
 * @param byte Byte.
 * @return True if an ACK was generated.
 */
bool I2C1Send(const uint8_t byte) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1STATbits.ACKSTAT = 0;
    I2C1TRN = byte;
    WaitForInterruptOrTimeout();
    const bool ack = I2C1STATbits.ACKSTAT == 0;
#ifdef PRINT_MESSAGES
    I2CPrintByte(byte);
    I2CPrintAckNack(ack);
#endif
    return ack;
}

/**
 * @brief Sends a 7-bit client address with appended R/W bit to indicate a
 * read.
 * @param address 7-bit client address.
 * @return True if an ACK was generated.
 */
bool I2C1SendAddressRead(const uint8_t address) {
    const bool ack = I2C1Send(I2CAddressRead(address));
#ifdef PRINT_MESSAGES
    I2CPrintReadAddress(address);
    I2CPrintAckNack(ack);
#endif
    return ack;
}

/**
 * @brief Sends a 7-bit client address with appended R/W bit to indicate a
 * write.
 * @param address 7-bit client address.
 * @return True if an ACK was generated.
 */
bool I2C1SendAddressWrite(const uint8_t address) {
    const bool ack = I2C1Send(I2CAddressWrite(address));
#ifdef PRINT_MESSAGES
    I2CPrintWriteAddress(address);
    I2CPrintAckNack(ack);
#endif
    return ack;
}

/**
 * @brief Receives a byte and generates an ACK or NACK.
 * @param ack True for ACK.
 * @return Byte.
 */
uint8_t I2C1Receive(const bool ack) {

    // Receive
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1CONbits.RCEN = 1;
    WaitForInterruptOrTimeout();

    // ACK/NACK
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1CONbits.ACKDT = ack ? 0 : 1;
    I2C1CONbits.ACKEN = 1;
    WaitForInterruptOrTimeout();
    const uint8_t byte = I2C1RCV;
#ifdef PRINT_MESSAGES
    I2CPrintByte(byte);
    I2CPrintAckNack(ack);
#endif
    return byte;
}

/**
 * @brief Waits for the interrupt or timeout.
 */
static void WaitForInterruptOrTimeout(void) {
    const uint64_t timeout = TimerGetTicks64() + I2C_TIMEOUT;
    while (true) {
        if (EVIC_SourceStatusGet(INT_SOURCE_I2C1_MASTER)) {
            break;
        }
        if (TimerGetTicks64() > timeout) {
            break;
        }
    }
}

//------------------------------------------------------------------------------
// End of file
