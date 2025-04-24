/**
 * @file I2CBB2.c
 * @author Seb Madgwick
 * @brief I2C bit-bang driver.
 */

//------------------------------------------------------------------------------
// Includes

#include "I2CBB.h"
#include "I2CBB2.h"
#include "I2CBBConfig.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Uncomment this line to enable printing of messages.
 */
//#define PRINT_MESSAGES

//------------------------------------------------------------------------------
// Variables

const I2C i2cBB2 = {
    .start = I2CBB2Start,
    .repeatedStart = I2CBB2RepeatedStart,
    .stop = I2CBB2Stop,
    .send = I2CBB2Send,
    .sendAddressRead = I2CBB2SendAddressRead,
    .sendAddressWrite = I2CBB2SendAddressWrite,
    .receive = I2CBB2Receive,
};
static const I2CBB i2cBB = {
    .sclPin = I2CBB2_SCL_PIN,
    .sdaPin = I2CBB2_SDA_PIN,
    .halfClockCycle = 5, // 100 kHz
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Performs the bus clear procedure. This procedure should be performed
 * if the SDA line is stuck low.
 * See page 20 of UM10204 I2C-bus specification and user manual Rev. 6 - 4 April
 * 2014.
 * @param i2cBB I2C bit bang structure.
 */
void I2CBB2BusClear(void) {
    I2CBBBusClear(&i2cBB);
}

/**
 * @brief Generates a start event.
 */
void I2CBB2Start(void) {
    I2CBBStart(&i2cBB);
#ifdef PRINT_MESSAGES
    I2CPrintStart();
#endif
}

/**
 * @brief Generates a repeated start event.
 */
void I2CBB2RepeatedStart(void) {
    I2CBBRepeatedStart(&i2cBB);
#ifdef PRINT_MESSAGES
    I2CPrintRepeatedStart();
#endif
}

/**
 * @brief Generates a stop event.
 */
void I2CBB2Stop(void) {
    I2CBBStop(&i2cBB);
#ifdef PRINT_MESSAGES
    I2CPrintStop();
#endif
}

/**
 * @brief Sends a byte and checks for ACK.
 * @param byte Byte.
 * @return True if an ACK was generated.
 */
bool I2CBB2Send(const uint8_t byte) {
    const bool ack = I2CBBSend(&i2cBB, byte);
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
bool I2CBB2SendAddressRead(const uint8_t address) {
    const bool ack = I2CBBSendAddressRead(&i2cBB, address);
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
bool I2CBB2SendAddressWrite(const uint8_t address) {
    const bool ack = I2CBBSendAddressWrite(&i2cBB, address);
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
uint8_t I2CBB2Receive(const bool ack) {
    const uint8_t byte = I2CBBReceive(&i2cBB, ack);
#ifdef PRINT_MESSAGES
    I2CPrintByte(byte);
    I2CPrintAckNack(ack);
#endif
    return byte;
}

//------------------------------------------------------------------------------
// End of file
