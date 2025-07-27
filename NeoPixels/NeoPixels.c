/**
 * @file NeoPixels.c
 * @author Seb Madgwick
 * @brief NeoPixels driver.
 */

//------------------------------------------------------------------------------
// Includes

#include "NeoPixels.h"

//------------------------------------------------------------------------------
// Variables

const SpiSettings neoPixelsSpiSettings = {
    .clockFrequency = 3333333, // 4 bits = 1.2 us
    .clockPolarity = SpiClockPolarityIdleHigh,
    .clockPhase = SpiClockPhaseIdleToActive,
};

//------------------------------------------------------------------------------
// End of file
