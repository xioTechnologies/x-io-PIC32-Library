/**
 * @file Pwm1.h
 * @author Seb Madgwick
 * @brief PWM driver for PIC32 devices.
 */

#ifndef PWM_1_H
#define PWM_1_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void Pwm1Initialise(const uint16_t dutyCycle);
void Pwm1Deinitialise(void);
uint16_t Pwm1Get(void);
void Pwm1Set(const uint16_t dutyCycle);

#endif

//------------------------------------------------------------------------------
// End of file
