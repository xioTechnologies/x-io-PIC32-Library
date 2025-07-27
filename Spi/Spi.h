/**
 * @file Spi.h
 * @author Seb Madgwick
 * @brief SPI driver for PIC32 devices.
 */

#ifndef SPI_H
#define SPI_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Clock polarity. Values equal the CKP bit of the SPIxCON register.
 */
typedef enum {
    SpiClockPolarityIdleLow,
    SpiClockPolarityIdleHigh,
} SpiClockPolarity;

/**
 * @brief Clock phase. Values equal the CKE bit of the SPIxCON register.
 */
typedef enum {
    SpiClockPhaseIdleToActive,
    SpiClockPhaseActiveToIdle,
} SpiClockPhase;

/**
 * @brief Settings.
 */
typedef struct {
    uint32_t clockFrequency;
    SpiClockPolarity clockPolarity;
    SpiClockPhase clockPhase;
} SpiSettings;

/**
 * @brief SPI interface.
 */
typedef struct {
    void (*transfer)(const GPIO_PIN csPin, void* const data, const size_t numberOfBytes, void (*transferComplete)(void));
    bool (*transferInProgress)(void);
} Spi;

//------------------------------------------------------------------------------
// Variable declarations

extern const SpiSettings spiSettingsDefault;

//------------------------------------------------------------------------------
// Function declarations

uint32_t SpiCalculateSpixbrg(const uint32_t clockFrequency);
float SpiCalculateClockFrequency(const uint32_t spixbrg);
void SpiPrintTransfer(GPIO_PIN csPin, const void * const data, const size_t numberOfBytes);
void SpiPrintTransferComplete(const void * const data, const size_t numberOfBytes);

#endif

//------------------------------------------------------------------------------
// End of file
