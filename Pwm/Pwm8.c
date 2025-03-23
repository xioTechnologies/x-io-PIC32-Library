/**
 * @file Pwm8.c
 * @author Seb Madgwick
 * @brief PWM driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Pwm8.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param dutyCycle Duty cycle.
 */
void Pwm8Initialise(const uint16_t dutyCycle) {

    // Ensure default register states
    Pwm8Deinitialise();

    // Set initial duty cycle
#if defined __PIC32MM__
    CCP4RB = dutyCycle;
#else
    OC8RS = dutyCycle;
#endif

    // Configure timer
#if defined __PIC32MM__
    CCP4CON1bits.MOD = 0b0100; // Dual Edge Compare mode
    CCP4CON1bits.ON = 1;
#else
    OC8R = 0xFFFF;
    OC8CONbits.OCM = 0b110; // PWM mode
    OC8CONbits.ON = 1;
#endif
}

/**
 * @brief Deinitialises the module.
 */
void Pwm8Deinitialise(void) {
#if defined __PIC32MM__
    CCP4CON1 = 0;
    CCP4CON2 = 0x01000000;
    CCP4CON3 = 0;
    CCP4STAT = 0;
#else
    OC8CON = 0;
#endif
}

/**
 * @brief Sets the duty cycle.
 * @param dutyCycle Duty cycle.
 */
void Pwm8Set(const uint16_t dutyCycle) {
#if defined __PIC32MM__
    CCP4RB = dutyCycle;
#else
    OC8RS = dutyCycle;
#endif
}

//------------------------------------------------------------------------------
// End of file
