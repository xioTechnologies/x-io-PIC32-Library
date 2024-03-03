/**
 * @file OnChange.h
 * @author Seb Madgwick
 * @brief Digital input change notification.
 */

#ifndef ON_CHANGE_H
#define ON_CHANGE_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stdint.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Edge.
 */
typedef enum {
    OnChangeEdgeNone,
    OnChangeEdgeLowToHigh,
    OnChangeEdgeHighToLow,
} OnChangeEdge;

/**
 * @brief On change structure.  Structure members are used internally and must
 * not be accessed by the application.
 */
typedef struct {
    bool previousInput;
    uint64_t timeout;
} OnChange;


/**
 * @brief Returns the edge of the digital input change.
 * @param input Input.
 * @return Edge of the digital input change.
 */
#define ON_CHANGE_POLL(input) ({ \
    static OnChange onChange; \
    OnChangePoll(&onChange, 0, input); \
})

/**
 * @brief Returns the edge of the digital input change with debounce.
 * @param input Input.
 * @return Edge of the digital input change.
 */
#define ON_CHANGE_POLL_DEBOUNCE(input) ({ \
    static OnChange onChange; \
    OnChangePoll(&onChange, 20, input); \
})

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Returns the edge of the digital input change.
 * @param onChange On change structure.
 * @param debouncePeriod Debounce period in milliseconds.
 * @param input Input.
 * @return Edge of the digital input change.
 */
static inline __attribute__((always_inline)) OnChangeEdge OnChangePoll(OnChange * const onChange, const uint32_t debouncePeriod, const bool input) {
    if (input == onChange->previousInput) {
        return OnChangeEdgeNone;
    }
    if (debouncePeriod == 0) {
        onChange->previousInput = input;
        return input ? OnChangeEdgeLowToHigh : OnChangeEdgeHighToLow;
    }
    const uint64_t ticks = TimerGetTicks64();
    if (ticks < onChange->timeout) {
        return OnChangeEdgeNone;
    }
    onChange->previousInput = input;
    onChange->timeout = ticks + (debouncePeriod * TIMER_TICKS_PER_MILLISECOND);
    return input ? OnChangeEdgeLowToHigh : OnChangeEdgeHighToLow;
}

#endif

//------------------------------------------------------------------------------
// End of file
