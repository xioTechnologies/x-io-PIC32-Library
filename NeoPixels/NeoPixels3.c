/**
 * @file NeoPixels3.c
 * @author Seb Madgwick
 * @brief NeoPixels driver.
 */

//------------------------------------------------------------------------------
// Includes

#include "BitPattern.h"
#include "NeoPixels3.h"
#include <string.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief SPI data.
 */
typedef struct {
    BitPattern pixels[NEOPIXELS_3_HAL_NUMBER_OF_PIXELS];
#ifndef NEOPIXELS_3_NO_RESET_CODE
    uint8_t resetCode[35]; // 84 us
#endif
} __attribute__((__packed__)) SpiData;

//------------------------------------------------------------------------------
// Variables

NeoPixelsPixel neoPixels3Pixels[NEOPIXELS_3_HAL_NUMBER_OF_PIXELS];
static __attribute__((coherent)) SpiData spiData; // data must be declared __attribute__((coherent)) for DMA transfers on PIC32MZ devices
volatile void* const neoPixels3SpiData = &spiData;
const size_t neoPixels3SpiNumberOfBytes = sizeof (spiData);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Updates the NeoPixels.
 */
void NeoPixels3Update(void) {

    // Wait for previous transfer to complete
#ifdef NEOPIXELS_3_SPI
    while (NEOPIXELS_3_SPI.transferInProgress());
#endif

    // Encode data
    for (int index = 0; index < NEOPIXELS_3_HAL_NUMBER_OF_PIXELS; index++) {
        const NeoPixelsPixel pixel = neoPixels3Pixels[index];
        spiData.pixels[index] = BitPatternFrom(pixel.red, pixel.green, pixel.blue);
    }

    // Clear reset code bytes
#ifndef NEOPIXELS_3_NO_RESET_CODE
    memset(spiData.resetCode, 0, sizeof (spiData.resetCode));
#endif

    // Begin transfer
#ifdef NEOPIXELS_3_SPI
    NEOPIXELS_3_SPI.transfer(GPIO_PIN_NONE, &spiData, sizeof (spiData), NULL);
#endif
}

//------------------------------------------------------------------------------
// End of file
