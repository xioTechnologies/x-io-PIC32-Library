/**
 * @file Pwm9.h
 * @author Seb Madgwick
 * @brief PWM driver for PIC32 devices.
 */

#ifndef PWM_9_H
#define PWM_9_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void Pwm9Initialise(const uint16_t dutyCycle);
void Pwm9Deinitialise(void);
uint16_t Pwm9Get(void);
void Pwm9Set(const uint16_t dutyCycle);

#endif

//------------------------------------------------------------------------------
// End of file
