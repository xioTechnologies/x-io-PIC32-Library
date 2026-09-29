/**
 * @file I2CStartSequence.c
 * @author Seb Madgwick
 * @brief I2C start sequence with acknowledge polling and timeout.
 */

//------------------------------------------------------------------------------
// Includes

#include "I2CStartSequence.h"
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Performs I2C start sequence with acknowledge polling and timeout.
 * @param i2c I2C interface.
 * @param address 7-bit client address.
 * @param timeout Timeout in milliseconds.
 * @return True if an ACK was received.
 */
bool I2CStartSequence(const I2C * const i2c, const uint8_t address, const uint32_t timeout) {
    const uint64_t timeoutTicks = TimerGetTicks64() + ((uint64_t) timeout * (uint64_t) TIMER_TICKS_PER_MILLISECOND);
    while (true) {
        i2c->start();
        if (i2c->transmitAddressWrite(address)) {
            return true;
        }
        if (TimerGetTicks64() > timeoutTicks) {
            return false;
        }
    }
}

//------------------------------------------------------------------------------
// End of file
