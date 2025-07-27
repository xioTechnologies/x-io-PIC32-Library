/**
 * @file Periodic.h
 * @author Seb Madgwick
 * @brief Periodic event scheduler.
 */

#ifndef PERIODIC_H
#define PERIODIC_H

//------------------------------------------------------------------------------
// Includes

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Periodic structure. All structure members are private.
 */
typedef struct {
    bool enabled;
    float periodSeconds;
    uint64_t periodTicks;
    uint64_t timeout;
} Periodic;

/**
 * @brief Returns true if the period has elapsed. The period starts on the
 * first function call. A period of zero or infinity will always return false.
 * @param periodic Periodic structure.
 * @param period Period in seconds.
 * @return True if the period has elapsed.
 */
#define PERIODIC_POLL(period) ({ \
    static Periodic periodic; \
    PeriodicPoll(&periodic, period); \
})

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Returns true if the period has elapsed. The period starts on the
 * first function call. A period of zero or infinity will always return false.
 * @param periodic Periodic structure.
 * @param period Period in seconds.
 * @return True if the period has elapsed.
 */
static inline __attribute__((always_inline)) bool PeriodicPoll(Periodic * const periodic, const float period) {

    // Recalculate if period changed
    if (periodic->periodSeconds != period) {

        // Disable if period invalid, zero, or infinity
        periodic->periodSeconds = period;
        periodic->enabled = false;
        if (period <= 0.0f) {
            return false;
        }
        if (isinf(period) != 0) {
            return false;
        }
        periodic->enabled = true;

        // Calculate internal variables
        periodic->periodTicks = (uint64_t) (period * (float) TIMER_TICKS_PER_SECOND);
        periodic->timeout = TimerGetTicks64() + periodic->periodTicks;
    }

    // Do nothing else if disabled
    if (periodic->enabled == false) {
        return false;
    }

    // Do nothing else if period has not elapsed
    const uint64_t ticks = TimerGetTicks64();
    if (ticks < periodic->timeout) {
        return false;
    }

    // Calculate time of next event
    periodic->timeout += periodic->periodTicks;
    if (periodic->timeout < ticks) {
        periodic->timeout = ticks + periodic->periodTicks;
    }
    return true;
}

#endif

//------------------------------------------------------------------------------
// End of file
