/**
 * @file NeoPixels5.h
 * @author Seb Madgwick
 * @brief NeoPixels driver.
 */

#ifndef NEOPIXELS_5_H
#define NEOPIXELS_5_H

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "NeoPixels.h"
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern NeoPixelsPixel neoPixels5Pixels[NEOPIXELS_5_HAL_NUMBER_OF_PIXELS];
#ifndef NEOPIXELS_5_SPI
extern volatile void* const neoPixels5SpiData;
extern const size_t neoPixels5SpiNumberOfBytes;
#endif

//------------------------------------------------------------------------------
// Function declarations

void NeoPixels5Update(void);

#endif

//------------------------------------------------------------------------------
// End of file
