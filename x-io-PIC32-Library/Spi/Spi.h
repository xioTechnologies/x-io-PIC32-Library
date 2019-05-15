/**
 * @file Spi.h
 * @author Seb Madgwick
 * @brief SPI driver for PIC32 devices.
 */

#ifndef SPI_H
#define SPI_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief SPI clock polarity enumeration.  Values equal the CKP bit of the
 * SPIxCON register.
 */
typedef enum {
    SpiClockPolarityIdleLow,
    SpiClockPolarityIdleHigh,
} SpiClockPolarity;

/**
 * @brief SPI clock phase enumeration.  Values equal the CKE bit of the
 * SPIxCON register.
 */
typedef enum {
    SpiClockPhaseIdleToActive,
    SpiClockPhaseActiveToIdle,
} SpiClockPhase;

/**
 * @brief SPI settings structure.
 */
typedef struct {
    uint32_t clockFrequency;
    SpiClockPolarity clockPolarity;
    SpiClockPhase clockPhase;
} SpiSettings;

//------------------------------------------------------------------------------
// Variable declarations

extern const SpiSettings spiSettingsDefault;

//------------------------------------------------------------------------------
// Function prototypes

uint32_t SpiCalculateSpixbrg(const uint32_t clockFrequency);

#endif

//------------------------------------------------------------------------------
// End of file
