/**
 * @file I2C5.c
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "I2C5.h"
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

const I2C i2c5 = {
    .start = I2C5Start,
    .repeatedStart = I2C5RepeatedStart,
    .stop = I2C5Stop,
    .send = I2C5Send,
    .sendAddressRead = I2C5SendAddressRead,
    .sendAddressWrite = I2C5SendAddressWrite,
    .receive = I2C5Receive,
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param clockFrequency Clock frequency.
 */
void I2C5Initialise(const I2CClockFrequency clockFrequency) {

    // Ensure default register states
    I2C5Deinitialise();

    // Configure I2C
    I2C5BRG = I2CCalculateI2Cxbrg(clockFrequency);
    if (clockFrequency != I2CClockFrequency400kHz) {
        I2C5CONbits.DISSLW = 1; // slew rate control disabled
    }
    I2C5CONbits.I2CEN = 1;
}

/**
 * @brief Disable the module.
 */
void I2C5Deinitialise(void) {

    // Disable I2C and restore default register states
    I2C5CON = 0;
    I2C5STAT = 0;

    // Disable interrupt
    EVIC_SourceDisable(INT_SOURCE_I2C5_MASTER);
    EVIC_SourceStatusClear(INT_SOURCE_I2C5_MASTER);
}

/**
 * @brief Generates a start event.
 */
void I2C5Start(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C5_MASTER);
    I2C5CONbits.SEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintStart();
#endif
}

/**
 * @brief Generates a repeated start event.
 */
void I2C5RepeatedStart(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C5_MASTER);
    I2C5CONbits.RSEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintRepeatedStart();
#endif
}

/**
 * @brief Generates a stop event.
 */
void I2C5Stop(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C5_MASTER);
    I2C5CONbits.PEN = 1;
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
bool I2C5Send(const uint8_t byte) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C5_MASTER);
    I2C5STATbits.ACKSTAT = 0;
    I2C5TRN = byte;
    WaitForInterruptOrTimeout();
    const bool ack = I2C5STATbits.ACKSTAT == 0;
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
bool I2C5SendAddressRead(const uint8_t address) {
    const bool ack = I2C5Send(I2CAddressRead(address));
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
bool I2C5SendAddressWrite(const uint8_t address) {
    const bool ack = I2C5Send(I2CAddressWrite(address));
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
uint8_t I2C5Receive(const bool ack) {

    // Receive
    EVIC_SourceStatusClear(INT_SOURCE_I2C5_MASTER);
    I2C5CONbits.RCEN = 1;
    WaitForInterruptOrTimeout();

    // ACK/NACK
    EVIC_SourceStatusClear(INT_SOURCE_I2C5_MASTER);
    I2C5CONbits.ACKDT = ack ? 0 : 1;
    I2C5CONbits.ACKEN = 1;
    WaitForInterruptOrTimeout();
    const uint8_t byte = I2C5RCV;
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
        if (EVIC_SourceStatusGet(INT_SOURCE_I2C5_MASTER)) {
            break;
        }
        if (TimerGetTicks64() > timeout) {
            break;
        }
    }
}

//------------------------------------------------------------------------------
// End of file
