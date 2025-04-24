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
 * @param timeout_ Timeout in milliseconds.
 * @return True if an ACK was generated.
 */
bool I2CStartSequence(const I2C * const i2c, const uint8_t address, const uint32_t timeout_) {
    const uint64_t timeout = TimerGetTicks64() + ((uint64_t) timeout_ * (uint64_t) TIMER_TICKS_PER_MILLISECOND);
    while (true) {
        i2c->start();
        if (i2c->sendAddressWrite(address)) {
            return true;
        }
        if (TimerGetTicks64() > timeout) {
            return false;
        }
    }
}

//------------------------------------------------------------------------------
// End of file
