/**
 * @file NeoPixels6.h
 * @author Seb Madgwick
 * @brief NeoPixels driver.
 */

#ifndef NEOPIXELS_6_H
#define NEOPIXELS_6_H

//------------------------------------------------------------------------------
// Includes

#include "NeoPixels.h"
#include "NeoPixelsConfig.h"
#include <stddef.h>

//------------------------------------------------------------------------------
// Variable declarations

extern NeoPixelsPixel neoPixels6Pixels[NEOPIXELS_6_HAL_NUMBER_OF_PIXELS];
#ifndef NEOPIXELS_6_SPI
extern void* const neoPixels6SpiData;
extern const size_t neoPixels6SpiNumberOfBytes;
#endif

//------------------------------------------------------------------------------
// Function declarations

void NeoPixels6Update(void);

#endif

//------------------------------------------------------------------------------
// End of file
