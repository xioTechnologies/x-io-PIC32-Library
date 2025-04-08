/**
 * @file Pwm5.h
 * @author Seb Madgwick
 * @brief PWM driver for PIC32 devices.
 */

#ifndef PWM_5_H
#define PWM_5_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void Pwm5Initialise(const uint16_t dutyCycle);
void Pwm5Deinitialise(void);
uint16_t Pwm5Get(void);
void Pwm5Set(const uint16_t dutyCycle);

#endif

//------------------------------------------------------------------------------
// End of file
