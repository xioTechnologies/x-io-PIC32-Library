/**
 * @file NeoPixels6.c
 * @author Seb Madgwick
 * @brief NeoPixels driver.
 */

//------------------------------------------------------------------------------
// Includes

#include "BitPattern.h"
#include "NeoPixels6.h"
#include <string.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief SPI data.
 */
typedef struct {
    BitPattern pixels[NEOPIXELS_6_HAL_NUMBER_OF_PIXELS];
#ifndef NEOPIXELS_6_NO_RESET_CODE
    uint8_t resetCode[35]; // 84 us
#endif
} __attribute__((__packed__)) SpiData;

//------------------------------------------------------------------------------
// Variables

NeoPixelsPixel neoPixels6Pixels[NEOPIXELS_6_HAL_NUMBER_OF_PIXELS];
static __attribute__((coherent)) SpiData spiData; // data must be declared __attribute__((coherent)) for DMA transfers on PIC32MZ devices
volatile void* const neoPixels6SpiData = &spiData;
const size_t neoPixels6SpiNumberOfBytes = sizeof (spiData);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Updates the NeoPixels.
 */
void NeoPixels6Update(void) {

    // Wait for previous transfer to complete
#ifdef NEOPIXELS_6_SPI
    while (NEOPIXELS_6_SPI.transferInProgress());
#endif

    // Encode data
    for (int index = 0; index < NEOPIXELS_6_HAL_NUMBER_OF_PIXELS; index++) {
        const NeoPixelsPixel pixel = neoPixels6Pixels[index];
        spiData.pixels[index] = BitPatternFrom(pixel.red, pixel.green, pixel.blue);
    }

    // Clear reset code bytes
#ifndef NEOPIXELS_6_NO_RESET_CODE
    memset(spiData.resetCode, 0, sizeof (spiData.resetCode));
#endif

    // Begin transfer
#ifdef NEOPIXELS_6_SPI
    NEOPIXELS_6_SPI.transfer(GPIO_PIN_NONE, &spiData, sizeof (spiData), NULL);
#endif
}

//------------------------------------------------------------------------------
// End of file
