/**
 * @file Uart.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "system_definitions.h"
#include "Uart.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Value used for UxBRG calculation.
 */
#if defined __PIC32MX__
#define FPB (SYS_CLK_BUS_PERIPHERAL_1)
#elif defined __PIC32MZ__
#define FPB (SYS_CLK_BUS_PERIPHERAL_2)
#else
#error "Unsupported device."
#endif

//------------------------------------------------------------------------------
// Variables

const UartSettings uartSettingsDefault = {
    .baudRate = 115200,
    .rtsCtsEnabled = false,
    .parityAndData = EightBitNoParity,
    .stopBits = OneStopBit,
    .invertDataLines = false,
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Calculates a UxBRG value for a specified baud rate.
 * See page 12 of Section 21. UART.
 * @param baudRate Baud rate.
 * @return UxBRG value.
 */
uint32_t UartCalculateUxbrg(const uint32_t baudRate) {
    const float idealUxbrg = ((float) FPB / (4.0f * (float) baudRate)) - 1.0f;
    return (uint32_t) (idealUxbrg + 0.5f);
}

//------------------------------------------------------------------------------
// End of file
