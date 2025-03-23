/**
 * @file Pwm3.h
 * @author Seb Madgwick
 * @brief PWM driver for PIC32 devices.
 */

#ifndef PWM_3_H
#define PWM_3_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void Pwm3Initialise(const uint16_t dutyCycle);
void Pwm3Deinitialise(void);
void Pwm3Set(const uint16_t dutyCycle);

#endif

//------------------------------------------------------------------------------
// End of file
