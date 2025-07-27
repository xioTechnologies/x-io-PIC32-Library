/**
 * @file NeoPixels3.h
 * @author Seb Madgwick
 * @brief NeoPixels driver.
 */

#ifndef NEOPIXELS_3_H
#define NEOPIXELS_3_H

//------------------------------------------------------------------------------
// Includes

#include "NeoPixels.h"
#include "NeoPixelsConfig.h"
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern NeoPixelsPixel neoPixels3Pixels[NEOPIXELS_3_HAL_NUMBER_OF_PIXELS];
#ifndef NEOPIXELS_3_SPI
extern void* const neoPixels3SpiData;
extern const size_t neoPixels3SpiNumberOfBytes;
#endif

//------------------------------------------------------------------------------
// Function declarations

void NeoPixels3Update(void);

#endif

//------------------------------------------------------------------------------
// End of file
