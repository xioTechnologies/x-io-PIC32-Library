/**
 * @file I2C2.c
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "I2C2.h"
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

const I2C i2c2 = {
    .start = I2C2Start,
    .repeatedStart = I2C2RepeatedStart,
    .stop = I2C2Stop,
    .send = I2C2Send,
    .sendAddressRead = I2C2SendAddressRead,
    .sendAddressWrite = I2C2SendAddressWrite,
    .receive = I2C2Receive,
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param clockFrequency Clock frequency.
 */
void I2C2Initialise(const I2CClockFrequency clockFrequency) {

    // Ensure default register states
    I2C2Deinitialise();

    // Configure I2C
    I2C2BRG = I2CCalculateI2Cxbrg(clockFrequency);
    if (clockFrequency != I2CClockFrequency400kHz) {
        I2C2CONbits.DISSLW = 1; // slew rate control disabled
    }
    I2C2CONbits.I2CEN = 1;
}

/**
 * @brief Disable the module.
 */
void I2C2Deinitialise(void) {

    // Disable I2C and restore default register states
    I2C2CON = 0;
    I2C2STAT = 0;

    // Disable interrupt
    EVIC_SourceDisable(INT_SOURCE_I2C2_MASTER);
    EVIC_SourceStatusClear(INT_SOURCE_I2C2_MASTER);
}

/**
 * @brief Generates a start event.
 */
void I2C2Start(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C2_MASTER);
    I2C2CONbits.SEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintStart();
#endif
}

/**
 * @brief Generates a repeated start event.
 */
void I2C2RepeatedStart(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C2_MASTER);
    I2C2CONbits.RSEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintRepeatedStart();
#endif
}

/**
 * @brief Generates a stop event.
 */
void I2C2Stop(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C2_MASTER);
    I2C2CONbits.PEN = 1;
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
bool I2C2Send(const uint8_t byte) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C2_MASTER);
    I2C2STATbits.ACKSTAT = 0;
    I2C2TRN = byte;
    WaitForInterruptOrTimeout();
    const bool ack = I2C2STATbits.ACKSTAT == 0;
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
bool I2C2SendAddressRead(const uint8_t address) {
    const bool ack = I2C2Send(I2CAddressRead(address));
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
bool I2C2SendAddressWrite(const uint8_t address) {
    const bool ack = I2C2Send(I2CAddressWrite(address));
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
uint8_t I2C2Receive(const bool ack) {

    // Receive
    EVIC_SourceStatusClear(INT_SOURCE_I2C2_MASTER);
    I2C2CONbits.RCEN = 1;
    WaitForInterruptOrTimeout();

    // ACK/NACK
    EVIC_SourceStatusClear(INT_SOURCE_I2C2_MASTER);
    I2C2CONbits.ACKDT = ack ? 0 : 1;
    I2C2CONbits.ACKEN = 1;
    WaitForInterruptOrTimeout();
    const uint8_t byte = I2C2RCV;
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
        if (EVIC_SourceStatusGet(INT_SOURCE_I2C2_MASTER)) {
            break;
        }
        if (TimerGetTicks64() > timeout) {
            break;
        }
    }
}

//------------------------------------------------------------------------------
// End of file
