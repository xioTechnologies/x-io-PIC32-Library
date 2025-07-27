/**
 * @file NeoPixels5.c
 * @author Seb Madgwick
 * @brief NeoPixels driver.
 */

//------------------------------------------------------------------------------
// Includes

#include "BitPattern.h"
#include "NeoPixels5.h"
#include <string.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief SPI data.
 */
typedef struct {
    BitPattern pixels[NEOPIXELS_5_HAL_NUMBER_OF_PIXELS];
    uint8_t resetCode[35]; // 84 us
} __attribute__((__packed__)) SpiData;

//------------------------------------------------------------------------------
// Variables

NeoPixelsPixel neoPixels5Pixels[NEOPIXELS_5_HAL_NUMBER_OF_PIXELS];
static __attribute__((coherent)) SpiData spiData; // data must be declared __attribute__((coherent)) for DMA transfers on PIC32MZ devices
void* const neoPixels5SpiData = &spiData;
const size_t neoPixels5SpiNumberOfBytes = sizeof (spiData);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Updates the NeoPixels.
 */
void NeoPixels5Update(void) {

    // Wait for previous transfer to complete
#ifdef NEOPIXELS_5_SPI
    while (NEOPIXELS_5_SPI.transferInProgress());
#endif

    // Encode data
    for (int index = 0; index < NEOPIXELS_5_HAL_NUMBER_OF_PIXELS; index++) {
        const NeoPixelsPixel pixel = neoPixels5Pixels[index];
        spiData.pixels[index] = BitPatternFrom(pixel.red, pixel.green, pixel.blue);
    }

    // Clear reset code bytes
    memset(spiData.resetCode, 0, sizeof (spiData.resetCode));

    // Begin transfer
#ifdef NEOPIXELS_5_SPI
    NEOPIXELS_5_SPI.transfer(GPIO_PIN_NONE, &spiData, sizeof (spiData), NULL);
#endif
}

//------------------------------------------------------------------------------
// End of file
