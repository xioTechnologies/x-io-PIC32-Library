/**
 * @file Timer.h
 * @author Seb Madgwick
 * @brief Timer driver for PIC32 devices.
 */

#ifndef TIMER_H
#define TIMER_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>
#include "system_definitions.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @bief Timer ticks per second.
 */
#if defined __PIC32MX__
#define TIMER_TICKS_PER_SECOND (SYS_CLK_BUS_PERIPHERAL_1)
#elif defined __PIC32MZ__
#define TIMER_TICKS_PER_SECOND (SYS_CLK_BUS_PERIPHERAL_3)
#else
#error "Unsupported device."
#endif

//------------------------------------------------------------------------------
// Function declarations

void TimerInitialise();
void TimerDisable();
uint32_t TimerGetTicks32();
uint64_t TimerGetTicks64();
void TimerDelay(const uint32_t milliseconds);
void TimerDelayMicroseconds(const uint32_t microseconds);

#endif

//------------------------------------------------------------------------------
// End of file
