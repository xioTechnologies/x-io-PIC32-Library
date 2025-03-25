/**
 * @file NeoPixels.c
 * @author Seb Madgwick
 * @brief Driver for NeoPixels.
 */

//------------------------------------------------------------------------------
// Includes

#include "HexToBitPattern.h"
#include "NeoPixels.h"
#include "NeoPixelsHal.h"
#include <stddef.h>
#include <string.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Bit pattern to encode RGB data.
 */
typedef struct {
    uint32_t green;
    uint32_t red;
    uint32_t blue;
} __attribute__((__packed__)) BitPattern;

/**
 * @brief SPI data to be transfered.
 */
typedef struct {
    BitPattern pixels[NEOPIXELS_HAL_NUMBER_OF_PIXELS];
    uint8_t resetCode[35]; // 84 us
} __attribute__((__packed__)) SpiData;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module. This function must only be called once, on
 * system startup.
 */
void NeoPixelsInitialise(void) {
    SpiSettings settings = spiSettingsDefault;
    settings.clockFrequency = 3333333; // 4 bits = 1.2 us
    NeoPixelsHalSpiInitialise(&settings);
}

/**
 * @brief Sets the NeoPixels.
 * @param pixels Pixels. The array length must be NEOPIXELS_NUMBER_OF_PIXELS.
 */
void NeoPixelsSet(const NeoPixelsPixel * const pixels) {

    // Wait for previous transfer to complete
    while (NeoPixelsHalSpiTransferInProgress());

    // Encode data
    static __attribute__((coherent)) SpiData spiData; // data must be declared __attribute__((coherent)) for DMA transfers on PIC32MZ devices
    for (int index = 0; index < NEOPIXELS_HAL_NUMBER_OF_PIXELS; index++) {
        spiData.pixels[index].red = hexToBitPattern[pixels[index].red];
        spiData.pixels[index].green = hexToBitPattern[pixels[index].green];
        spiData.pixels[index].blue = hexToBitPattern[pixels[index].blue];
    }

    // Clear reset code bytes
    memset(spiData.resetCode, 0, sizeof (spiData.resetCode));

    // Begin transfer
    NeoPixelsHalSpiTransfer(GPIO_PIN_NONE, &spiData, sizeof (spiData), NULL);
}

//------------------------------------------------------------------------------
// End of file
