/**
 * @file SpiBusConfig.h
 * @author Seb Madgwick
 * @brief SPI bus.
 */

#ifndef SPI_BUS_CONFIG_H
#define SPI_BUS_CONFIG_H

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

#define SPI_BUS_1_MAX_NUMBER_OF_CLIENTS (6)
#define SPI_BUS_1_SPI                   spi1Dma

#define SPI_BUS_2_MAX_NUMBER_OF_CLIENTS (4)
#define SPI_BUS_2_SPI                   spi2Dma

#define SPI_BUS_3_MAX_NUMBER_OF_CLIENTS (4)
#define SPI_BUS_3_SPI                   spi3Dma

#define SPI_BUS_4_MAX_NUMBER_OF_CLIENTS (4)
#define SPI_BUS_4_SPI                   spi4Dma

#define SPI_BUS_5_MAX_NUMBER_OF_CLIENTS (4)
#define SPI_BUS_5_SPI                   spi5Dma

#define SPI_BUS_6_MAX_NUMBER_OF_CLIENTS (4)
#define SPI_BUS_6_SPI                   spi6Dma

#endif

//------------------------------------------------------------------------------
// End of file
