/**
 * @file Spi.h
 * @author Seb Madgwick
 * @brief SPI driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "PeripheralBusClockFrequency.h"
#include "Spi.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Value used for SPIxBRG calculation.
 */
#if defined __PIC32MX__
#define FPB (PERIPHERAL_BUS_CLOCK_FREQUENCY)
#elif defined __PIC32MZ__
#define FPB (PERIPHERAL_BUS_CLOCK_2_FREQUENCY)
#else
#error "Unsupported device."
#endif

//------------------------------------------------------------------------------
// Variables

const SpiSettings spiSettingsDefault = {
    .clockFrequency = 5000000,
    .clockPolarity = SpiClockPolarityIdleHigh,
    .clockPhase = SpiClockPhaseIdleToActive,
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Calculates a SPIxBRG value for a specified clock frequency.
 * See page 29 of Section 23. Serial Peripheral Interface (SPI).
 * @param clockFrequency Clock frequency in Hz.
 * @return SPIxBRG values.
 */
uint32_t SpiCalculateSpixbrg(const uint32_t clockFrequency) {
    const float idealSpixbrg = ((float) FPB / (2.0f * (float) clockFrequency)) - 1.0f;
    return (uint32_t) (idealSpixbrg + 0.5f);
}

//------------------------------------------------------------------------------
// End of file
