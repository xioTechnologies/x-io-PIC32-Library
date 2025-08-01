/**
 * @file NeoPixels4.h
 * @author Seb Madgwick
 * @brief NeoPixels driver.
 */

#ifndef NEOPIXELS_4_H
#define NEOPIXELS_4_H

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "NeoPixels.h"
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern NeoPixelsPixel neoPixels4Pixels[NEOPIXELS_4_HAL_NUMBER_OF_PIXELS];
#ifndef NEOPIXELS_4_SPI
extern volatile void* const neoPixels4SpiData;
extern const size_t neoPixels4SpiNumberOfBytes;
#endif

//------------------------------------------------------------------------------
// Function declarations

void NeoPixels4Update(void);

#endif

//------------------------------------------------------------------------------
// End of file
