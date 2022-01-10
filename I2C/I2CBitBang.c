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
static inline __attribute__((always_inline)) void WriteScl(const bool state);
static inline __attribute__((always_inline)) bool ReadSda();
static inline __attribute__((always_inline)) void WriteSda(const bool state);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Performs the bus clear procedure.  This procedure should be performed
 * if the SDA line is stuck low.
 * See page 20 of UM10204 I2C-bus specification and user manual Rev. 6 - 4 April
 * 2014.
 */
void I2CBitBangBusClear() {
    int index;
    for (index = 0; index < 9; index++) {
        WaitHalfClockCycle();
        if (ReadSda() == true) { // sample data during clock high period
            break; // stop once SDA is released otherwise it may get stuck again
        }
        WriteScl(false);
        WaitHalfClockCycle();
        WriteScl(true);
    }
}

/**
 * @brief Generates a start event.
 */
void I2CBitBangStart() {
    WriteScl(true);
    WriteSda(true);
    WaitHalfClockCycle();
    WriteSda(false);
    WaitHalfClockCycle();
    WriteScl(false);
}

/**
 * @brief Generates a repeated start event.
 */
void I2CBitBangRepeatedStart() {
    WriteScl(false);
    WriteSda(true);
    WaitHalfClockCycle();
    WriteScl(true);
    WaitHalfClockCycle();
    WriteSda(false);
    WaitHalfClockCycle();
    WriteScl(false);
}

/**
 * @brief Generates a stop event.
 */
void I2CBitBangStop() {
    WriteSda(false);
    WaitHalfClockCycle();
    WriteScl(true);
    WaitHalfClockCycle();
    WriteSda(true);
}

/**
 * @brief Sends a byte and checks for ACK.
 * @param byte Byte.
 * @return True if an ACK was generated.
 */
bool I2CBitBangSend(const uint8_t byte) {

    // Data
    int bitNumber;
    for (bitNumber = 7; bitNumber >= 0; bitNumber--) {
        WriteSda((byte & (1 << bitNumber)) != 0);
        WaitHalfClockCycle();
        WriteScl(true);
        WaitHalfClockCycle();
        WriteScl(false);
    }

    // ACK
    WriteSda(true);
    WaitHalfClockCycle();
    WriteScl(true);
    WaitHalfClockCycle();
    const bool ack = ReadSda() == false;
    WriteScl(false);
    WriteSda(false);
    return ack;
}

/**
 * @brief Receives a byte and generates an ACK or NACK.
 * @param ack True for ACK.
 * @return Byte.
 */
uint8_t I2CBitBangReceive(const bool ack) {

    // Data
    WriteSda(true);
    uint8_t byte = 0;
    int bitNumber;
    for (bitNumber = 7; bitNumber >= 0; bitNumber--) {
        WaitHalfClockCycle();
        WriteScl(true);
        WaitHalfClockCycle();
        byte |= ReadSda() == true ? (1 << bitNumber) : 0;
        WriteScl(false);
    }

    // ACK/NACK
    WriteSda(ack == false);
    WaitHalfClockCycle();
    WriteScl(true);
    WaitHalfClockCycle();
    WriteScl(false);
    return byte;
}

/**
 * @brief Delays for half a clock cycle.
 */
static inline __attribute__((always_inline)) void WaitHalfClockCycle() {
    TimerDelayMicroseconds(2);
}

/**
 * @brief Writes the SCL pin.
 * @param state Pin state.
 */
static inline __attribute__((always_inline)) void WriteScl(const bool state) {
    if (state) {
        SCL_Set();
    } else {
        SCL_Clear();
    }
}

/**
 * @brief Reads the SDA pin.
 * @return Pin state.
 */
static inline __attribute__((always_inline)) bool ReadSda() {
    return SDA_Get();
}

/**
 * @brief Writes the SDA pin.
 * @param state Pin state.
 */
static inline __attribute__((always_inline)) void WriteSda(const bool state) {
    if (state) {
        SDA_Set();
    } else {
        SDA_Clear();
    }
}

//------------------------------------------------------------------------------
// End of file
