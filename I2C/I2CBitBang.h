/**
 * @file I2CBitBang.h
 * @author Seb Madgwick
 * @brief I2C bit-bang driver.
 */

#ifndef I2C_BIT_BANG_H
#define I2C_BIT_BANG_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include <stdbool.h>
#include <stdint.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief I2C bit bang structure.
 */
typedef struct {
    GPIO_PIN scl;
    GPIO_PIN sda;
    uint32_t halfClockCycle; /* microseconds */
} I2CBitBang;

//------------------------------------------------------------------------------
// Function declarations

/**
 * @brief Performs the bus clear procedure.  This procedure should be performed
 * if the SDA line is stuck low.
 * See page 20 of UM10204 I2C-bus specification and user manual Rev. 6 - 4 April
 * 2014.
 * @param i2cBitBang I2C bit bang structure.
 */
static inline __attribute__((always_inline)) void I2CBitBangBusClear(const I2CBitBang * const i2cBitBang) {
    for (int index = 0; index < 9; index++) {
        TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
        if (GPIO_PinRead(i2cBitBang->sda)) { // sample data during clock high period
            break; // stop once SDA is released otherwise it may get stuck again
        }
        GPIO_PinWrite(i2cBitBang->scl, false);
        TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
        GPIO_PinWrite(i2cBitBang->scl, true);
    }
}

/**
 * @brief Generates a start event.
 * @param i2cBitBang I2C bit bang structure.
 */
static inline __attribute__((always_inline)) void I2CBitBangStart(const I2CBitBang * const i2cBitBang) {
    GPIO_PinWrite(i2cBitBang->scl, true);
    GPIO_PinWrite(i2cBitBang->sda, true);
    TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
    GPIO_PinWrite(i2cBitBang->sda, false);
    TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
    GPIO_PinWrite(i2cBitBang->scl, false);
}

/**
 * @brief Generates a repeated start event.
 * @param i2cBitBang I2C bit bang structure.
 */
static inline __attribute__((always_inline)) void I2CBitBangRepeatedStart(const I2CBitBang * const i2cBitBang) {
    GPIO_PinWrite(i2cBitBang->scl, false);
    GPIO_PinWrite(i2cBitBang->sda, true);
    TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
    GPIO_PinWrite(i2cBitBang->scl, true);
    TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
    GPIO_PinWrite(i2cBitBang->sda, false);
    TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
    GPIO_PinWrite(i2cBitBang->scl, false);
}

/**
 * @brief Generates a stop event.
 * @param i2cBitBang I2C bit bang structure.
 */
static inline __attribute__((always_inline)) void I2CBitBangStop(const I2CBitBang * const i2cBitBang) {
    GPIO_PinWrite(i2cBitBang->sda, false);
    TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
    GPIO_PinWrite(i2cBitBang->scl, true);
    TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
    GPIO_PinWrite(i2cBitBang->sda, true);
}

/**
 * @brief Sends a byte and checks for ACK.
 * @param i2cBitBang I2C bit bang structure.
 * @param byte Byte.
 * @return True if an ACK was generated.
 */
static inline __attribute__((always_inline)) bool I2CBitBangSend(const I2CBitBang * const i2cBitBang, const uint8_t byte) {

    // Data
    for (int bitNumber = 7; bitNumber >= 0; bitNumber--) {
        GPIO_PinWrite(i2cBitBang->sda, (byte & (1 << bitNumber)) != 0);
        TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
        GPIO_PinWrite(i2cBitBang->scl, true);
        TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
        GPIO_PinWrite(i2cBitBang->scl, false);
    }

    // ACK
    GPIO_PinWrite(i2cBitBang->sda, true);
    TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
    GPIO_PinWrite(i2cBitBang->scl, true);
    TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
    const bool ack = GPIO_PinRead(i2cBitBang->sda) == false;
    GPIO_PinWrite(i2cBitBang->scl, false);
    GPIO_PinWrite(i2cBitBang->sda, false);
    return ack;
}

/**
 * @brief Receives a byte and generates an ACK or NACK.
 * @param i2cBitBang I2C bit bang structure.
 * @param ack True for ACK.
 * @return Byte.
 */
static inline __attribute__((always_inline)) uint8_t I2CBitBangReceive(const I2CBitBang * const i2cBitBang, const bool ack) {

    // Data
    GPIO_PinWrite(i2cBitBang->sda, true);
    uint8_t byte = 0;
    for (int bitNumber = 7; bitNumber >= 0; bitNumber--) {
        TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
        GPIO_PinWrite(i2cBitBang->scl, true);
        TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
        byte |= GPIO_PinRead(i2cBitBang->sda) ? (1 << bitNumber) : 0;
        GPIO_PinWrite(i2cBitBang->scl, false);
    }

    // ACK/NACK
    GPIO_PinWrite(i2cBitBang->sda, ack == false);
    TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
    GPIO_PinWrite(i2cBitBang->scl, true);
    TimerDelayMicroseconds(i2cBitBang->halfClockCycle);
    GPIO_PinWrite(i2cBitBang->scl, false);
    return byte;
}

#endif

//------------------------------------------------------------------------------
// End of file
