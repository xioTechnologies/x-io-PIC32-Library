/**
 * @file PeripheralBusClockFrequency.h
 * @author Seb Madgwick
 * @brief Peripheral bus clock frequency.
 */

#ifndef PERIPHERAL_BUS_CLOCK_FREQUENCY_H
#define PERIPHERAL_BUS_CLOCK_FREQUENCY_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"

//------------------------------------------------------------------------------
// Definitions

#if (defined __PIC32MX__) || (defined __PIC32MM__)
#define PERIPHERAL_BUS_CLOCK_FREQUENCY (CPU_CLOCK_FREQUENCY)
#elif defined __PIC32MZ__
#if CPU_CLOCK_FREQUENCY > 200000000U
#define PBDIV 3
#else
#define PBDIV 2
#endif
#define PERIPHERAL_BUS_CLOCK_1_FREQUENCY (CPU_CLOCK_FREQUENCY / PBDIV)
#define PERIPHERAL_BUS_CLOCK_2_FREQUENCY (CPU_CLOCK_FREQUENCY / PBDIV)
#define PERIPHERAL_BUS_CLOCK_3_FREQUENCY (CPU_CLOCK_FREQUENCY / PBDIV)
#define PERIPHERAL_BUS_CLOCK_4_FREQUENCY (CPU_CLOCK_FREQUENCY / PBDIV)
#define PERIPHERAL_BUS_CLOCK_5_FREQUENCY (CPU_CLOCK_FREQUENCY / PBDIV)
#define PERIPHERAL_BUS_CLOCK_6_FREQUENCY (CPU_CLOCK_FREQUENCY / PBDIV)
#define PERIPHERAL_BUS_CLOCK_7_FREQUENCY (CPU_CLOCK_FREQUENCY / 1)
#define PERIPHERAL_BUS_CLOCK_8_FREQUENCY (CPU_CLOCK_FREQUENCY / PBDIV)
#else
#error "Unsupported device."
#endif

#endif

//------------------------------------------------------------------------------
// End of file
