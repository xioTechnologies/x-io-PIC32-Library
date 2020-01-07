/**
 * @file Timer.c
 * @author Seb Madgwick
 * @brief Timer driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "Timer.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Variables

static uint32_t timerOverflowCounter;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 */
void TimerInitialise() {

    // Ensure default register states
    TimerDisable();

    // Configure timer
    T4CONbits.T32 = 1;
    T4CONbits.ON = 1;

    // Configure interrupt
    SYS_INT_VectorPrioritySet(_TIMER_5_VECTOR, INT_PRIORITY_LEVEL7);
    SYS_INT_SourceStatusClear(INT_SOURCE_TIMER_5);
    SYS_INT_SourceEnable(INT_SOURCE_TIMER_5);
}

/**
 * @brief Disables the module.
 */
void TimerDisable() {

    // Disable timer and restore default register states
    T4CON = 0;
    T5CON = 0;

    // Disable interrupt
    SYS_INT_SourceDisable(INT_SOURCE_TIMER_5);
}

/**
 * @brief Gets the 32-bit timer value.
 * @return 32-bit timer value.
 */
uint32_t TimerGetTicks32() {
    return TMR4; // read TMR4 and TMR5 as 32-bit timer
}

/**
 * @brief Gets the 64-bit timer value.
 * @return 64-bit timer value.
 */
uint64_t TimerGetTicks64() {

    typedef union {
        uint64_t value;

        struct {
            uint32_t dword0; // least-significant dword
            uint32_t dword1; // most-significant dword
        };
    } Uint64Union;

    Uint64Union uint64Union;
    do {
        uint64Union.dword1 = timerOverflowCounter; // must read this value first
        uint64Union.dword0 = TMR4; // read 32-bit timer value
    } while (uint64Union.dword1 != timerOverflowCounter); // avoid seconds overflow hazard
    return uint64Union.value;
}

/**
 * @brief Blocking delay in milliseconds.
 * @param milliseconds Delay in milliseconds.
 */
void TimerDelay(const uint32_t milliseconds) {
    uint64_t currentTicks = TimerGetTicks64();
    const uint64_t endTicks = currentTicks + ((uint64_t) milliseconds * (uint64_t) (TIMER_TICKS_PER_SECOND / 1000));
    do {
        currentTicks = TimerGetTicks64();
    } while (currentTicks < endTicks);
}

/**
 * @brief Blocking delay in microseconds.  This function should not be used for
 * delays approaching 2^32 microseconds.
 * @param microseconds Delay in microseconds.
 */
void TimerDelayMicroseconds(const uint32_t microseconds) {
    const uint32_t startTicks = TimerGetTicks32();
    uint32_t currentTicks;
    do {
        currentTicks = TimerGetTicks32();
    } while ((currentTicks - startTicks) < (microseconds * (TIMER_TICKS_PER_SECOND / 1000000)));
}

/**
 * @brief Timer overflow interrupt to increment overflow counter.
 */
void __ISR(_TIMER_5_VECTOR) Timer5Interrupt() {
    timerOverflowCounter++;
    SYS_INT_SourceStatusClear(INT_SOURCE_TIMER_5);
}

//------------------------------------------------------------------------------
// End of file
