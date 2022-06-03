/**
 * @file i2cBitBang.c
 * @author Seb Madgwick
 * @brief I2C bit-bang driver.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "i2cBitBang.h"
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Function declarations

static inline __attribute__((always_inline)) void WaitHalfClockCycle();

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Performs the bus clear procedure.  This procedure should be performed
 * if the SDA line is stuck low.
 * See page 20 of UM10204 I2C-bus specification and user manual Rev. 6 - 4 April
 * 2014.
 */
void I2CBitBangBusClear() {
    for (int index = 0; index < 9; index++) {
        WaitHalfClockCycle();
        if (GPIO_PinRead(SDA_PIN) == true) { // sample data during clock high period
            break; // stop once SDA is released otherwise it may get stuck again
        }
        GPIO_PinWrite(SCL_PIN, false);
        WaitHalfClockCycle();
        GPIO_PinWrite(SCL_PIN, true);
    }
}

/**
 * @brief Generates a start event.
 */
void I2CBitBangStart() {
    GPIO_PinWrite(SCL_PIN, true);
    GPIO_PinWrite(SDA_PIN, true);
    WaitHalfClockCycle();
    GPIO_PinWrite(SDA_PIN, false);
    WaitHalfClockCycle();
    GPIO_PinWrite(SCL_PIN, false);
}

/**
 * @brief Generates a repeated start event.
 */
void I2CBitBangRepeatedStart() {
    GPIO_PinWrite(SCL_PIN, false);
    GPIO_PinWrite(SDA_PIN, true);
    WaitHalfClockCycle();
    GPIO_PinWrite(SCL_PIN, true);
    WaitHalfClockCycle();
    GPIO_PinWrite(SDA_PIN, false);
    WaitHalfClockCycle();
    GPIO_PinWrite(SCL_PIN, false);
}

/**
 * @brief Generates a stop event.
 */
void I2CBitBangStop() {
    GPIO_PinWrite(SDA_PIN, false);
    WaitHalfClockCycle();
    GPIO_PinWrite(SCL_PIN, true);
    WaitHalfClockCycle();
    GPIO_PinWrite(SDA_PIN, true);
}

/**
 * @brief Sends a byte and checks for ACK.
 * @param byte Byte.
 * @return True if an ACK was generated.
 */
bool I2CBitBangSend(const uint8_t byte) {

    // Data
    for (int bitNumber = 7; bitNumber >= 0; bitNumber--) {
        GPIO_PinWrite(SDA_PIN, (byte & (1 << bitNumber)) != 0);
        WaitHalfClockCycle();
        GPIO_PinWrite(SCL_PIN, true);
        WaitHalfClockCycle();
        GPIO_PinWrite(SCL_PIN, false);
    }

    // ACK
    GPIO_PinWrite(SDA_PIN, true);
    WaitHalfClockCycle();
    GPIO_PinWrite(SCL_PIN, true);
    WaitHalfClockCycle();
    const bool ack = GPIO_PinRead(SDA_PIN) == false;
    GPIO_PinWrite(SCL_PIN, false);
    GPIO_PinWrite(SDA_PIN, false);
    return ack;
}

/**
 * @brief Receives a byte and generates an ACK or NACK.
 * @param ack True for ACK.
 * @return Byte.
 */
uint8_t I2CBitBangReceive(const bool ack) {

    // Data
    GPIO_PinWrite(SDA_PIN, true);
    uint8_t byte = 0;
    for (int bitNumber = 7; bitNumber >= 0; bitNumber--) {
        WaitHalfClockCycle();
        GPIO_PinWrite(SCL_PIN, true);
        WaitHalfClockCycle();
        byte |= GPIO_PinRead(SDA_PIN) == true ? (1 << bitNumber) : 0;
        GPIO_PinWrite(SCL_PIN, false);
    }

    // ACK/NACK
    GPIO_PinWrite(SDA_PIN, ack == false);
    WaitHalfClockCycle();
    GPIO_PinWrite(SCL_PIN, true);
    WaitHalfClockCycle();
    GPIO_PinWrite(SCL_PIN, false);
    return byte;
}

/**
 * @brief Delays for half a clock cycle.
 */
static inline __attribute__((always_inline)) void WaitHalfClockCycle() {
    TimerDelayMicroseconds(2);
}

//------------------------------------------------------------------------------
// End of file
