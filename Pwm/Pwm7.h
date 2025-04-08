/**
 * @file Pwm7.h
 * @author Seb Madgwick
 * @brief PWM driver for PIC32 devices.
 */

#ifndef PWM_7_H
#define PWM_7_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void Pwm7Initialise(const uint16_t dutyCycle);
void Pwm7Deinitialise(void);
uint16_t Pwm7Get(void);
void Pwm7Set(const uint16_t dutyCycle);

#endif

//------------------------------------------------------------------------------
// End of file
