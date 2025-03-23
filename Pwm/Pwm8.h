/**
 * @file Pwm8.h
 * @author Seb Madgwick
 * @brief PWM driver for PIC32 devices.
 */

#ifndef PWM_8_H
#define PWM_8_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void Pwm8Initialise(const uint16_t dutyCycle);
void Pwm8Deinitialise(void);
void Pwm8Set(const uint16_t dutyCycle);

#endif

//------------------------------------------------------------------------------
// End of file
