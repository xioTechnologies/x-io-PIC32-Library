/**
 * @file Timer.h
 * @author Seb Madgwick
 * @brief Timer driver for PIC32 devices.
 */

#ifndef TIMER_H
#define TIMER_H

//------------------------------------------------------------------------------
// Includes

#include "PeripheralBusClockFrequency.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Timer ticks per second.
 */
#if (defined __PIC32MX__) || (defined __PIC32MM__)
#define TIMER_TICKS_PER_SECOND (PERIPHERAL_BUS_CLOCK_FREQUENCY)
#elif defined __PIC32MZ__
#define TIMER_TICKS_PER_SECOND (PERIPHERAL_BUS_CLOCK_3_FREQUENCY)
#else
#error "Unsupported device."
#endif

/**
 * @brief Timer ticks per millisecond.
 */
#define TIMER_TICKS_PER_MILLISECOND (TIMER_TICKS_PER_SECOND / 1000)

/**
 * @brief Timer ticks per microsecond.
 */
#define TIMER_TICKS_PER_MICROSECOND (TIMER_TICKS_PER_SECOND / 1000000)

//------------------------------------------------------------------------------
// Function declarations

void TimerInitialise(void);
void TimerDeinitialise(void);
uint32_t TimerGetTicks32(void);
uint64_t TimerGetTicks64(void);
void TimerDelayMilliseconds(const uint32_t milliseconds);
void TimerDelayMicroseconds(const uint32_t microseconds);

#endif

//------------------------------------------------------------------------------
// End of file
