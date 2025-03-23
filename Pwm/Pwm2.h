/**
 * @file Pwm2.h
 * @author Seb Madgwick
 * @brief PWM driver for PIC32 devices.
 */

#ifndef PWM_2_H
#define PWM_2_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void Pwm2Initialise(const uint16_t dutyCycle);
void Pwm2Deinitialise(void);
void Pwm2Set(const uint16_t dutyCycle);

#endif

//------------------------------------------------------------------------------
// End of file
