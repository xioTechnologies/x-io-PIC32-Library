/**
 * @file I2CStartSequence.c
 * @author Seb Madgwick
 * @brief I2C start sequence with acknowledge polling and timeout.
 */

//------------------------------------------------------------------------------
// Includes

#include "I2CSlaveAddress.h"
#include "I2CStartSequence.h"
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Performs I2C start sequence with acknowledge polling and timeout.
 * @param i2cStart I2C start function.
 * @param i2cSend I2C send function.
 * @param byte Byte.
 * @param timeout_ Timeout in milliseconds.
 * @return True if an ACK was generated.
 */
bool I2CStartSequence(void (*i2cStart)(void), bool(*i2cSend)(const uint8_t byte), const uint8_t slaveAddress, const uint32_t timeout_) {
    const uint64_t timeout = TimerGetTicks64() + ((uint64_t) timeout_ * (uint64_t) TIMER_TICKS_PER_MILLISECOND);
    while (true) {
        i2cStart();
        if (i2cSend(I2CSlaveAddressWrite(slaveAddress)) == true) {
            return true;
        }
        if (TimerGetTicks64() > timeout) {
            return false;
        }
    }
}

//------------------------------------------------------------------------------
// End of file
