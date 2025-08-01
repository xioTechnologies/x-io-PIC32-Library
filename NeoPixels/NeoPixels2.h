/**
 * @file NeoPixels2.h
 * @author Seb Madgwick
 * @brief NeoPixels driver.
 */

#ifndef NEOPIXELS_2_H
#define NEOPIXELS_2_H

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "NeoPixels.h"
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern NeoPixelsPixel neoPixels2Pixels[NEOPIXELS_2_HAL_NUMBER_OF_PIXELS];
#ifndef NEOPIXELS_2_SPI
extern volatile void* const neoPixels2SpiData;
extern const size_t neoPixels2SpiNumberOfBytes;
#endif

//------------------------------------------------------------------------------
// Function declarations

void NeoPixels2Update(void);

#endif

//------------------------------------------------------------------------------
// End of file
