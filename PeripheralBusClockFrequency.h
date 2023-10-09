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
#define PERIPHERAL_BUS_CLOCK_1_FREQUENCY (CPU_CLOCK_FREQUENCY / 2)
#define PERIPHERAL_BUS_CLOCK_2_FREQUENCY (CPU_CLOCK_FREQUENCY / 2)
#define PERIPHERAL_BUS_CLOCK_3_FREQUENCY (CPU_CLOCK_FREQUENCY / 2)
#define PERIPHERAL_BUS_CLOCK_4_FREQUENCY (CPU_CLOCK_FREQUENCY / 2)
#define PERIPHERAL_BUS_CLOCK_5_FREQUENCY (CPU_CLOCK_FREQUENCY / 2)
#define PERIPHERAL_BUS_CLOCK_6_FREQUENCY (CPU_CLOCK_FREQUENCY / 2)
#define PERIPHERAL_BUS_CLOCK_7_FREQUENCY (CPU_CLOCK_FREQUENCY)
#define PERIPHERAL_BUS_CLOCK_8_FREQUENCY (CPU_CLOCK_FREQUENCY / 2)
#else
#error "Unsupported device."
#endif

#endif

//------------------------------------------------------------------------------
// End of file
