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
 * @bief Timer ticks per second.
 */
#if defined __PIC32MX__
#define TIMER_TICKS_PER_SECOND (PERIPHERAL_BUS_CLOCK_FREQUENCY)
#elif defined __PIC32MZ__
#define TIMER_TICKS_PER_SECOND (PERIPHERAL_BUS_CLOCK_3_FREQUENCY)
#else
#error "Unsupported device."
#endif

//------------------------------------------------------------------------------
// Function declarations

void TimerInitialise();
void TimerDisable();
uint32_t TimerGetTicks32();
uint64_t TimerGetTicks64();
void TimerDelayMilliseconds(const uint32_t milliseconds);
void TimerDelayMicroseconds(const uint32_t microseconds);

#endif

//------------------------------------------------------------------------------
// End of file
