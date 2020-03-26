/**
 * @file Uart.h
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

#ifndef UART_H
#define UART_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief UART parity and data.  Values equal to PDSEL bits of UxMODE register.
 */
typedef enum {
    UartParityAndDataEightNone,
    UartParityAndDataEightEven,
    UartParityAndDataEightOdd,
    UartParityAndDataNineNone,
} UartParityAndData;

/**
 * @brief UART stop bits.  Values equal to STSEL bits of UxMODE register.
 */
typedef enum {
    UartStopBitsOne,
    UartStopBitsTwo,
} UartStopBits;

/**
 * @brief UART settings.
 */
typedef struct {
    uint32_t baudRate;
    bool rtsCtsEnabled;
    UartParityAndData parityAndData;
    UartStopBits stopBits;
    bool invertDataLines;
} UartSettings;

//------------------------------------------------------------------------------
// Variable declarations

extern const UartSettings uartSettingsDefault;

//------------------------------------------------------------------------------
// Function declarations

uint32_t UartCalculateUxbrg(const uint32_t baudRate);

#endif

//------------------------------------------------------------------------------
// End of file
