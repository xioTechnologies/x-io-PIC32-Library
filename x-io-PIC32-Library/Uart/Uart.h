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
 * @brief UART parity and data enumeration.  Values equal to PDSEL bits of
 * UxMODE register.
 */
typedef enum {
    EightBitNoParity,
    EightBitEvenParity,
    EightBitOddParity,
    NineBitNoParity,
} UartParityAndData;

/**
 * @brief UART stop bits enumeration.  Values equal to STSEL bits of UxMODE
 * register.
 */
typedef enum {
    OneStopBit,
    TwoStopBits,
} UartStopBits;

/**
 * @brief UART settings structure.
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
// Function prototypes

uint32_t UartCalculateUxbrg(const uint32_t baudRate);

#endif

//------------------------------------------------------------------------------
// End of file
