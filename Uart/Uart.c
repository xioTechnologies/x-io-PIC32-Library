/**
 * @file Uart.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "PeripheralBusClockFrequency.h"
#include "Uart.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief UART peripheral clock frequency.
 */
#if (defined __PIC32MM__) || (defined __PIC32MX__)
#define UART_PERIPHERAL_CLOCK (PERIPHERAL_BUS_CLOCK_FREQUENCY)
#elif defined __PIC32MZ__
#define UART_PERIPHERAL_CLOCK (PERIPHERAL_BUS_CLOCK_2_FREQUENCY)
#else
#error "Unsupported device."
#endif

//------------------------------------------------------------------------------
// Variables

const UartSettings uartSettingsDefault = {
    .baudRate = 115200,
    .rtsCtsEnabled = false,
    .parityAndData = UartParityAndDataEightNone,
    .stopBits = UartStopBitsOne,
    .invertTXRX = false,
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Calculates the UxBRG value for a target baud rate.
 * See page 13 of Section 21. UART.
 * @param baudRate Baud rate.
 * @return UxBRG value.
 */
uint32_t UartCalculateUxbrg(const uint32_t baudRate) {
    const float idealUxbrg = ((float) UART_PERIPHERAL_CLOCK / (4.0f * (float) baudRate)) - 1.0f;
    return (uint32_t) (idealUxbrg + 0.5f);
}

/**
 * @brief Calculates the actual baud rate for a UxBRG value.
 * See page 13 of Section 21. UART.
 * @param uxbrg UxBRG value.
 * @return Baud rate.
 */
float UartCalculateBaudRate(const uint32_t uxbrg) {
    return (float) UART_PERIPHERAL_CLOCK / (4.0f * ((float) uxbrg + 1.0f));
}

//------------------------------------------------------------------------------
// End of file
