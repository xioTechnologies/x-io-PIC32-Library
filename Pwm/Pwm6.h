/**
 * @file Pwm6.h
 * @author Seb Madgwick
 * @brief PWM driver for PIC32 devices.
 */

#ifndef PWM_6_H
#define PWM_6_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void Pwm6Initialise(const uint16_t dutyCycle);
void Pwm6Deinitialise(void);
void Pwm6Set(const uint16_t dutyCycle);

#endif

//------------------------------------------------------------------------------
// End of file
