/**
 * @file NeoPixels.h
 * @author Seb Madgwick
 * @brief Driver for NeoPixels.
 */

#ifndef NEOPIXELS_H
#define NEOPIXELS_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief RGB pixel.
 */
typedef union {

    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha; // unused
    } __attribute__((__packed__));
    uint32_t rgb;
} NeoPixelsPixel;

//------------------------------------------------------------------------------
// Function declarations

void NeoPixelsInitialise(void);
void NeoPixelsSet(const NeoPixelsPixel * const pixels);

#endif

//------------------------------------------------------------------------------
// End of file
