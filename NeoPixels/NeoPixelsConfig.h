/**
 * @file NeoPixelsConfig.h
 * @author Seb Madgwick
 * @brief NeoPixels driver.
 */

#ifndef NEOPIXELS_CONFIG_H
#define NEOPIXELS_CONFIG_H

//------------------------------------------------------------------------------
// Includes

#include "Spi/Spi1Dma.h"
#include "Spi/Spi2Dma.h"
#include "Spi/Spi3Dma.h"
#include "Spi/Spi4Dma.h"
#include "Spi/Spi5Dma.h"
#include "Spi/Spi6Dma.h"

//------------------------------------------------------------------------------
// Definitions

#define NEOPIXELS_1_HAL_NUMBER_OF_PIXELS    (4)
#define NEOPIXELS_1_SPI                     spi1Dma

#define NEOPIXELS_2_HAL_NUMBER_OF_PIXELS    (4)
#define NEOPIXELS_2_SPI                     spi1Dma

#define NEOPIXELS_3_HAL_NUMBER_OF_PIXELS    (4)
#define NEOPIXELS_3_SPI                     spi1Dma

#define NEOPIXELS_4_HAL_NUMBER_OF_PIXELS    (4)
#define NEOPIXELS_4_SPI                     spi1Dma

#define NEOPIXELS_5_HAL_NUMBER_OF_PIXELS    (4)
#define NEOPIXELS_5_SPI                     spi1Dma

#define NEOPIXELS_6_HAL_NUMBER_OF_PIXELS    (4)
#define NEOPIXELS_6_SPI                     spi1Dma

#endif

//------------------------------------------------------------------------------
// End of file
