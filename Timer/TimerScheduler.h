/**
 * @file TimerScheduler.h
 * @author Seb Madgwick
 * @brief Scheduler for periodic events.
 */

#ifndef TIMER_SCHEDULER_H
#define TIMER_SCHEDULER_H

//------------------------------------------------------------------------------
// Includes

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Scheduler structure. Structure members are used internally and must
 * not be accessed by the application.
 */
typedef struct {
    bool enabled;
    float periodSeconds;
    uint64_t periodTicks;
    uint64_t timeout;
} TimerScheduler;

/**
 * @brief Returns true if the period has elapsed. The period starts on the
 * first function call. A period of zero or infinity will always return false.
 * @param scheduler Scheduler structure.
 * @param period Period in seconds.
 */
#define TIMER_SCHEDULER_POLL(period) ({ \
    static TimerScheduler scheduler; \
    TimerSchedulerPoll(&scheduler, period); \
})

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Returns true if the period has elapsed. The period starts on the
 * first function call. A period of zero or infinity will always return false.
 * @param scheduler Scheduler structure.
 * @param period Period in seconds.
 * @return True if the period has elapsed.
 */
static inline __attribute__((always_inline)) bool TimerSchedulerPoll(TimerScheduler * const scheduler, const float period) {

    // Recalculate if period changed
    if (scheduler->periodSeconds != period) {

        // Disable if period invalid, zero, or infinity
        scheduler->periodSeconds = period;
        scheduler->enabled = false;
        if (period <= 0.0f) {
            return false;
        }
        if (isinf(period) != 0) {
            return false;
        }
        scheduler->enabled = true;

        // Calculate internal variables
        scheduler->periodTicks = (uint64_t) (period * (float) TIMER_TICKS_PER_SECOND);
        scheduler->timeout = TimerGetTicks64() + scheduler->periodTicks;
    }

    // Do nothing else if disabled
    if (scheduler->enabled == false) {
        return false;
    }

    // Do nothing else if period has not elapsed
    const uint64_t ticks = TimerGetTicks64();
    if (ticks < scheduler->timeout) {
        return false;
    }

    // Calculate time of next event
    scheduler->timeout += scheduler->periodTicks;
    if (scheduler->timeout < ticks) {
        scheduler->timeout = ticks + scheduler->periodTicks;
    }
    return true;
}

#endif

//------------------------------------------------------------------------------
// End of file
