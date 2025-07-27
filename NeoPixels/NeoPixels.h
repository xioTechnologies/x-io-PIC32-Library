/**
 * @file NeoPixels.h
 * @author Seb Madgwick
 * @brief NeoPixels driver.
 */

#ifndef NEOPIXELS_H
#define NEOPIXELS_H

//------------------------------------------------------------------------------
// Includes

#include "Spi/Spi.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Pixel.
 */
typedef union {

    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        unsigned : 8;
    } __attribute__((__packed__));
    uint32_t rgb;
} NeoPixelsPixel;

//------------------------------------------------------------------------------
// Variable declarations

extern const SpiSettings neoPixelsSpiSettings;

#endif

//------------------------------------------------------------------------------
// End of file
