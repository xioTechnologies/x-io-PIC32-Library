/**
 * @file Pwm4.h
 * @author Seb Madgwick
 * @brief PWM driver for PIC32 devices.
 */

#ifndef PWM_4_H
#define PWM_4_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void Pwm4Initialise(const uint16_t dutyCycle);
void Pwm4Deinitialise(void);
uint16_t Pwm4Get(void);
void Pwm4Set(const uint16_t dutyCycle);

#endif

//------------------------------------------------------------------------------
// End of file
