/**
 * @file I2CBB2.c
 * @author Seb Madgwick
 * @brief I2C bit-bang driver.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "I2CBB.h"
#include "I2CBB2.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Uncomment this line to print messages.
 */
//#define PRINT_MESSAGES

//------------------------------------------------------------------------------
// Variables

const I2C i2cBB2 = {
    .start = I2CBB2Start,
    .repeatedStart = I2CBB2RepeatedStart,
    .stop = I2CBB2Stop,
    .transmit = I2CBB2Transmit,
    .transmitAddressRead = I2CBB2TransmitAddressRead,
    .transmitAddressWrite = I2CBB2TransmitAddressWrite,
    .receiveAck = I2CBB2ReceiveAck,
    .receiveNack = I2CBB2ReceiveNack,
};

static const I2CBB i2cBB = {
    .sclPin = I2CBB2_SCL_PIN,
    .sdaPin = I2CBB2_SDA_PIN,
    .halfClockCycle = I2CBB2_HALF_CLOCK_CYCLE,
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Performs the bus clear procedure. This procedure should be performed
 * if the SDA line is stuck low.
 * See page 20 of UM10204 I2C-bus specification and user manual Rev. 6 - 4 April
 * 2014.
 */
void I2CBB2BusClear(void) {
    I2CBBBusClear(&i2cBB);
}

/**
 * @brief Generates a start condition.
 */
void I2CBB2Start(void) {
    I2CBBStart(&i2cBB);
#ifdef PRINT_MESSAGES
    I2CPrintStart();
#endif
}

/**
 * @brief Generates a repeated start condition.
 */
void I2CBB2RepeatedStart(void) {
    I2CBBRepeatedStart(&i2cBB);
#ifdef PRINT_MESSAGES
    I2CPrintRepeatedStart();
#endif
}

/**
 * @brief Generates a stop condition.
 */
void I2CBB2Stop(void) {
    I2CBBStop(&i2cBB);
#ifdef PRINT_MESSAGES
    I2CPrintStop();
#endif
}

/**
 * @brief Transmits a byte and checks for an ACK.
 * @param byte Byte.
 * @return True if an ACK was received.
 */
bool I2CBB2Transmit(const uint8_t byte) {
    const bool ack = I2CBBTransmit(&i2cBB, byte);
#ifdef PRINT_MESSAGES
    I2CPrintByte(byte);
    if (ack) {
        I2CPrintAck();
    } else {
        I2CPrintNack();
    }
#endif
    return ack;
}

/**
 * @brief Transmits a 7-bit client address with appended R/W bit to indicate a
 * read.
 * @param address 7-bit client address.
 * @return True if an ACK was received.
 */
bool I2CBB2TransmitAddressRead(const uint8_t address) {
    const bool ack = I2CBBTransmit(&i2cBB, I2CAddressRead(address));
#ifdef PRINT_MESSAGES
    I2CPrintReadAddress(address);
    if (ack) {
        I2CPrintAck();
    } else {
        I2CPrintNack();
    }
#endif
    return ack;
}

/**
 * @brief Transmits a 7-bit client address with appended R/W bit to indicate a
 * write.
 * @param address 7-bit client address.
 * @return True if an ACK was received.
 */
bool I2CBB2TransmitAddressWrite(const uint8_t address) {
    const bool ack = I2CBBTransmit(&i2cBB, I2CAddressWrite(address));
#ifdef PRINT_MESSAGES
    I2CPrintWriteAddress(address);
    if (ack) {
        I2CPrintAck();
    } else {
        I2CPrintNack();
    }
#endif
    return ack;
}

/**
 * @brief Receives a byte and generates an ACK.
 * @return Byte.
 */
uint8_t I2CBB2ReceiveAck(void) {
    const uint8_t byte = I2CBBReceive(&i2cBB);
    I2CBBAck(&i2cBB);
#ifdef PRINT_MESSAGES
    I2CPrintByte(byte);
    I2CPrintAck();
#endif
    return byte;
}

/**
 * @brief Receives a byte and generates a NACK.
 * @return Byte.
 */
uint8_t I2CBB2ReceiveNack(void) {
    const uint8_t byte = I2CBBReceive(&i2cBB);
    I2CBBNack(&i2cBB);
#ifdef PRINT_MESSAGES
    I2CPrintByte(byte);
    I2CPrintNack();
#endif
    return byte;
}

//------------------------------------------------------------------------------
// End of file
