/**
 * @file TimerEvent.h
 * @author Seb Madgwick
 * @brief Schedules an event to repeat with a fixed period.
 */

#ifndef TIMER_EVENT_H
#define TIMER_EVENT_H

//------------------------------------------------------------------------------
// Includes

#include <math.h> // isinf
#include <stdbool.h>
#include <stdint.h>
#include "Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Timer event.  Structure members are used internally and must not be
 * accessed by the application.
 */
typedef struct {
    bool enabled;
    float periodSeconds;
    uint64_t periodTicks;
    uint64_t nextEventTicks;
} TimerEvent;

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Polls the timer event and returns true if the period has elapsed and
 * the event should be triggered.  The timer event structure must be declared as
 * static so that all member values are initialised to zero.
 * @param timerEvent Timer event structure.
 * @param period Period in seconds.  A value of zero or infinity will disable
 * the event.
 * @return True if the period has elapsed and the event should be triggered.
 */
static inline __attribute__((always_inline)) bool TimerEventPoll(TimerEvent * const timerEvent, const float period) {

    // Recalculate internal variables if period changed
    if (*(uint32_t*) & timerEvent->periodSeconds != *(uint32_t*) & period) { // fast float comparison

        // Disable if period invalid
        timerEvent->periodSeconds = period;
        timerEvent->enabled = false;
        if (period < 0.0f) {
            return false;
        }
        if (isinf(period) != 0) {
            return false;
        }
        timerEvent->enabled = true;

        // Calculate internal variables
        timerEvent->periodTicks = (uint64_t) (period * (float) TIMER_TICKS_PER_SECOND);
        timerEvent->nextEventTicks = TimerGetTicks64() + timerEvent->periodTicks;
    }

    // Do nothing else if disabled
    if (timerEvent->enabled == false) {
        return false;
    }

    // Do nothing else if period has not elapsed
    const uint64_t currentTicks = TimerGetTicks64();
    if (currentTicks < timerEvent->nextEventTicks) {
        return false;
    }

    // Calculate time of next event
    do {
        timerEvent->nextEventTicks += timerEvent->periodTicks;
    } while (timerEvent->nextEventTicks <= currentTicks); // ensure that the next event is in the future
    return true;
}

#endif

//------------------------------------------------------------------------------
// End of file
