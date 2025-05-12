/**
 * @file NeoPixelsHal.h
 * @author Seb Madgwick
 * @brief NeoPixels driver Hardware Abstraction Layer (HAL).
 */

#ifndef NEOPIXELS_HAL_H
#define NEOPIXELS_HAL_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi/Spi1DmaTX.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Number of NeoPixels.
 */
#define NEOPIXELS_HAL_NUMBER_OF_PIXELS 4

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 */
static inline __attribute__((always_inline)) void NeoPixelsHalSpiInitialise(const SpiSettings * const settings) {
    Spi1DmaTXInitialise(settings);
}

/**
 * @brief Transfers data. The data will be overwritten with the received data.
 * This function must not be called while a transfer is in progress. The
 * transfer complete callback will be called from within an interrupt once the
 * transfer is complete.
 * @param csPin_ CS pin.
 * @param data_ Data.
 * @param numberOfBytes_ Number of bytes.
 * @param transferComplete_ Transfer complete callback.
 */
static inline __attribute__((always_inline)) void NeoPixelsHalSpiTransfer(const GPIO_PIN csPin, void* const data, const size_t numberOfBytes, void (*transferComplete)(void)) {
    Spi1DmaTXTransfer(csPin, data, numberOfBytes, transferComplete);
}

/**
 * @brief Returns true while the transfer is in progress.
 * @return True while the transfer is in progress.
 */
static inline __attribute__((always_inline)) bool NeoPixelsHalSpiTransferInProgress(void) {
    return Spi1DmaTXTransferInProgress();
}

#endif

//------------------------------------------------------------------------------
// End of file
