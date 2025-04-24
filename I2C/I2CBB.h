/**
 * @file I2CBB.h
 * @author Seb Madgwick
 * @brief I2C bit-bang driver.
 */

#ifndef I2CBB_H
#define I2CBB_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "I2C.h"
#include <stdbool.h>
#include <stdint.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief I2C bit bang structure.
 */
typedef struct {
    const GPIO_PIN sclPin;
    const GPIO_PIN sdaPin;
    const uint32_t halfClockCycle; /* microseconds */
} I2CBB;

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Performs the bus clear procedure. This procedure should be performed
 * if the SDA line is stuck low.
 * See page 20 of UM10204 I2C-bus specification and user manual Rev. 6 - 4 April
 * 2014.
 * @param i2cBB I2C bit bang structure.
 */
static inline __attribute__((always_inline)) void I2CBBBusClear(const I2CBB * const i2cBB) {
    for (int index = 0; index < 9; index++) {
        TimerDelayMicroseconds(i2cBB->halfClockCycle);
        if (GPIO_PinRead(i2cBB->sdaPin)) { // sample data during clock high period
            break; // stop once SDA is released otherwise it may get stuck again
        }
        GPIO_PinWrite(i2cBB->sclPin, false);
        TimerDelayMicroseconds(i2cBB->halfClockCycle);
        GPIO_PinWrite(i2cBB->sclPin, true);
    }
}

/**
 * @brief Generates a start event.
 * @param i2cBB I2C bit bang structure.
 */
static inline __attribute__((always_inline)) void I2CBBStart(const I2CBB * const i2cBB) {
    GPIO_PinWrite(i2cBB->sclPin, true);
    GPIO_PinWrite(i2cBB->sdaPin, true);
    TimerDelayMicroseconds(i2cBB->halfClockCycle);
    GPIO_PinWrite(i2cBB->sdaPin, false);
    TimerDelayMicroseconds(i2cBB->halfClockCycle);
    GPIO_PinWrite(i2cBB->sclPin, false);
}

/**
 * @brief Generates a repeated start event.
 * @param i2cBB I2C bit bang structure.
 */
static inline __attribute__((always_inline)) void I2CBBRepeatedStart(const I2CBB * const i2cBB) {
    GPIO_PinWrite(i2cBB->sclPin, false);
    GPIO_PinWrite(i2cBB->sdaPin, true);
    TimerDelayMicroseconds(i2cBB->halfClockCycle);
    GPIO_PinWrite(i2cBB->sclPin, true);
    TimerDelayMicroseconds(i2cBB->halfClockCycle);
    GPIO_PinWrite(i2cBB->sdaPin, false);
    TimerDelayMicroseconds(i2cBB->halfClockCycle);
    GPIO_PinWrite(i2cBB->sclPin, false);
}

/**
 * @brief Generates a stop event.
 * @param i2cBB I2C bit bang structure.
 */
static inline __attribute__((always_inline)) void I2CBBStop(const I2CBB * const i2cBB) {
    GPIO_PinWrite(i2cBB->sdaPin, false);
    TimerDelayMicroseconds(i2cBB->halfClockCycle);
    GPIO_PinWrite(i2cBB->sclPin, true);
    TimerDelayMicroseconds(i2cBB->halfClockCycle);
    GPIO_PinWrite(i2cBB->sdaPin, true);
}

/**
 * @brief Sends a byte and checks for ACK.
 * @param i2cBB I2C bit bang structure.
 * @param byte Byte.
 * @return True if an ACK was generated.
 */
static inline __attribute__((always_inline)) bool I2CBBSend(const I2CBB * const i2cBB, const uint8_t byte) {

    // Data
    for (int bitIndex = 7; bitIndex >= 0; bitIndex--) {
        GPIO_PinWrite(i2cBB->sdaPin, (byte & (1 << bitIndex)) != 0);
        TimerDelayMicroseconds(i2cBB->halfClockCycle);
        GPIO_PinWrite(i2cBB->sclPin, true);
        TimerDelayMicroseconds(i2cBB->halfClockCycle);
        GPIO_PinWrite(i2cBB->sclPin, false);
    }

    // ACK
    GPIO_PinWrite(i2cBB->sdaPin, true);
    TimerDelayMicroseconds(i2cBB->halfClockCycle);
    GPIO_PinWrite(i2cBB->sclPin, true);
    TimerDelayMicroseconds(i2cBB->halfClockCycle);
    const bool ack = GPIO_PinRead(i2cBB->sdaPin) == false;
    GPIO_PinWrite(i2cBB->sclPin, false);
    GPIO_PinWrite(i2cBB->sdaPin, false);
    return ack;
}

/**
 * @brief Sends a 7-bit client address with appended R/W bit to indicate a
 * read.
 * @param i2cBB I2C bit bang structure.
 * @param address 7-bit client address.
 * @return True if an ACK was generated.
 */
static inline __attribute__((always_inline)) bool I2CBBSendAddressRead(const I2CBB * const i2cBB, const uint8_t address) {
    return I2CBBSend(i2cBB, I2CAddressRead(address));
}

/**
 * @brief Sends a 7-bit client address with appended R/W bit to indicate a
 * write.
 * @param i2cBB I2C bit bang structure.
 * @param address 7-bit client address.
 * @return True if an ACK was generated.
 */
static inline __attribute__((always_inline)) bool I2CBBSendAddressWrite(const I2CBB * const i2cBB, const uint8_t address) {
    return I2CBBSend(i2cBB, I2CAddressWrite(address));
}

/**
 * @brief Receives a byte and generates an ACK or NACK.
 * @param i2cBB I2C bit bang structure.
 * @param ack True for ACK.
 * @return Byte.
 */
static inline __attribute__((always_inline)) uint8_t I2CBBReceive(const I2CBB * const i2cBB, const bool ack) {

    // Data
    GPIO_PinWrite(i2cBB->sdaPin, true);
    uint8_t byte = 0;
    for (int bitIndex = 7; bitIndex >= 0; bitIndex--) {
        TimerDelayMicroseconds(i2cBB->halfClockCycle);
        GPIO_PinWrite(i2cBB->sclPin, true);
        TimerDelayMicroseconds(i2cBB->halfClockCycle);
        byte |= GPIO_PinRead(i2cBB->sdaPin) ? (1 << bitIndex) : 0;
        GPIO_PinWrite(i2cBB->sclPin, false);
    }

    // ACK/NACK
    GPIO_PinWrite(i2cBB->sdaPin, ack == false);
    TimerDelayMicroseconds(i2cBB->halfClockCycle);
    GPIO_PinWrite(i2cBB->sclPin, true);
    TimerDelayMicroseconds(i2cBB->halfClockCycle);
    GPIO_PinWrite(i2cBB->sclPin, false);
    return byte;
}

#endif

//------------------------------------------------------------------------------
// End of file
