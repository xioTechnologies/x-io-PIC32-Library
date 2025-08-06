/**
 * @file Config.h
 * @author Seb Madgwick
 * @brief Library configuration.
 */

#ifndef CONFIG_H
#define CONFIG_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi/Spi1Dma.h"
#include "Spi/Spi2Dma.h"
#include "Spi/Spi3Dma.h"
#include "Spi/Spi4Dma.h"
#include "Spi/Spi5Dma.h"
#include "Spi/Spi6Dma.h"

//------------------------------------------------------------------------------
// Definitions

#define DATA_LOGGER_BUFFER_SIZE             (393216)

#define I2CBB1_SCL_PIN                      SCL1_PIN
#define I2CBB1_SDA_PIN                      SDA1_PIN
#define I2CBB1_HALF_CLOCK_CYCLE             (5)

#define I2CBB2_SCL_PIN                      SCL2_PIN
#define I2CBB2_SDA_PIN                      SDA2_PIN
#define I2CBB2_HALF_CLOCK_CYCLE             (5)

#define NEOPIXELS_1_HAL_NUMBER_OF_PIXELS    (4)
//#define NEOPIXELS_1_NO_RESET_CODE
#define NEOPIXELS_1_SPI                     spi1Dma

#define NEOPIXELS_2_HAL_NUMBER_OF_PIXELS    (4)
//#define NEOPIXELS_2_NO_RESET_CODE
#define NEOPIXELS_2_SPI                     spi2Dma

#define NEOPIXELS_3_HAL_NUMBER_OF_PIXELS    (4)
//#define NEOPIXELS_3_NO_RESET_CODE
#define NEOPIXELS_3_SPI                     spi3Dma

#define NEOPIXELS_4_HAL_NUMBER_OF_PIXELS    (4)
//#define NEOPIXELS_4_NO_RESET_CODE
#define NEOPIXELS_4_SPI                     spi4Dma

#define NEOPIXELS_5_HAL_NUMBER_OF_PIXELS    (4)
//#define NEOPIXELS_5_NO_RESET_CODE
#define NEOPIXELS_5_SPI                     spi5Dma

#define NEOPIXELS_6_HAL_NUMBER_OF_PIXELS    (4)
//#define NEOPIXELS_6_NO_RESET_CODE
#define NEOPIXELS_6_SPI                     spi6Dma

//#define SPI1_CS_ACTIVE_HIGH

//#define SPI2_CS_ACTIVE_HIGH

//#define SPI3_CS_ACTIVE_HIGH

//#define SPI4_CS_ACTIVE_HIGH

//#define SPI5_CS_ACTIVE_HIGH

//#define SPI6_CS_ACTIVE_HIGH

#define SPI_BUS_1_MAX_NUMBER_OF_CLIENTS     (4)
#define SPI_BUS_1_SPI                       spi1Dma

#define SPI_BUS_2_MAX_NUMBER_OF_CLIENTS     (4)
#define SPI_BUS_2_SPI                       spi2Dma

#define SPI_BUS_3_MAX_NUMBER_OF_CLIENTS     (4)
#define SPI_BUS_3_SPI                       spi3Dma

#define SPI_BUS_4_MAX_NUMBER_OF_CLIENTS     (4)
#define SPI_BUS_4_SPI                       spi4Dma

#define SPI_BUS_5_MAX_NUMBER_OF_CLIENTS     (4)
#define SPI_BUS_5_SPI                       spi5Dma

#define SPI_BUS_6_MAX_NUMBER_OF_CLIENTS     (4)
#define SPI_BUS_6_SPI                       spi6Dma

#define UART1_READ_BUFFER_SIZE              (4096)
#define UART1_WRITE_BUFFER_SIZE             (4096)

#define UART2_READ_BUFFER_SIZE              (4096)
#define UART2_WRITE_BUFFER_SIZE             (4096)

#define UART3_READ_BUFFER_SIZE              (4096)
#define UART3_WRITE_BUFFER_SIZE             (4096)

#define UART4_READ_BUFFER_SIZE              (4096)
#define UART4_WRITE_BUFFER_SIZE             (4096)

#define UART5_READ_BUFFER_SIZE              (4096)
#define UART5_WRITE_BUFFER_SIZE             (4096)

#define UART6_READ_BUFFER_SIZE              (4096)
#define UART6_WRITE_BUFFER_SIZE             (4096)

#define USB_CDC_READ_BUFFER_SIZE            (4096)
#define USB_CDC_WRITE_BUFFER_SIZE           (4096)

#endif

//------------------------------------------------------------------------------
// End of file
