/**
 * @file Pwm3.c
 * @author Seb Madgwick
 * @brief PWM driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Pwm3.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param dutyCycle Duty cycle.
 */
void Pwm3Initialise(const uint16_t dutyCycle) {

    // Ensure default register states
    Pwm3Deinitialise();

    // Set initial duty cycle
#if defined __PIC32MM__
    CCP4RB = dutyCycle;
#else
    OC3RS = dutyCycle;
#endif

    // Configure timer
#if defined __PIC32MM__
    CCP4CON1bits.MOD = 0b0100; // dual Edge Compare mode
    CCP4CON1bits.ON = 1;
#else
    OC3R = 0xFFFF;
    OC3CONbits.OCM = 0b110; // PWM mode
    OC3CONbits.ON = 1;
#endif
}

/**
 * @brief Deinitialises the module.
 */
void Pwm3Deinitialise(void) {
#if defined __PIC32MM__
    CCP4CON1 = 0;
    CCP4CON2 = 0x01000000;
    CCP4CON3 = 0;
    CCP4STAT = 0;
#else
    OC3CON = 0;
#endif
}

/**
 * @brief Gets the duty cycle.
 * @return Duty cycle.
 */
uint16_t Pwm3Get(void) {
#if defined __PIC32MM__
    return CCP4RB;
#else
    return OC3RS;
#endif
}

/**
 * @brief Sets the duty cycle.
 * @param dutyCycle Duty cycle.
 */
void Pwm3Set(const uint16_t dutyCycle) {
#if defined __PIC32MM__
    CCP4RB = dutyCycle;
#else
    OC3RS = dutyCycle;
#endif
}

//------------------------------------------------------------------------------
// End of file
