/**
 * @file Uart1.c
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "CircularBuffer.h"
#include "definitions.h"
#include <stdint.h>
#include "Uart1.h"

//------------------------------------------------------------------------------
// Function declarations

static inline __attribute__((always_inline)) void RXInterruptTasks();
static inline __attribute__((always_inline)) void TXInterruptTasks();

//------------------------------------------------------------------------------
// Variables

static bool receiveBufferOverrun;
static uint8_t readBufferData[4096];
static CircularBuffer readBuffer = {.buffer = readBufferData, .bufferSize = sizeof (readBufferData)};
static uint8_t writeBufferData[4096];
static CircularBuffer writeBuffer = {.buffer = writeBufferData, .bufferSize = sizeof (writeBufferData)};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 */
void Uart1Initialise(const UartSettings * const settings) {

    // Ensure default register states
    Uart1Disable();

    // Configure UART
    if (settings->rtsCtsEnabled == true) {
        U1MODEbits.UEN = 0b10; // UxTX, UxRX, UxCTS and UxRTS pins are enabled and used
    }
    if (settings->invertTXRX == true) {
        U1MODEbits.RXINV = 1; // UxRX Idle state is '0'
        U1STAbits.UTXINV = 1; // UxTX Idle state is '0'
    }
    U1MODEbits.PDSEL = settings->parityAndData;
    U1MODEbits.STSEL = settings->stopBits;
    U1MODEbits.BRGH = 1; // High-Speed mode - 4x baud clock enabled
    U1STAbits.URXISEL = 0b01; // Interrupt flag bit is asserted while receive buffer is 1/2 or more full (i.e., has 4 or more data characters)
    U1STAbits.UTXISEL = 0b10; // Interrupt is generated and asserted while the transmit buffer is empty
    U1STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U1STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U1BRG = UartCalculateUxbrg(settings->baudRate);
    U1MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

    // Configure interrupts
    EVIC_SourceEnable(INT_SOURCE_UART1_RX); // enable RX interrupt only
}

/**
 * @brief Disables the module.
 */
void Uart1Disable() {

    // Disable UART and restore default register states
    U1MODE = 0;
    U1STA = 0;

    // Disable interrupts
    EVIC_SourceDisable(INT_SOURCE_UART1_RX);
    EVIC_SourceDisable(INT_SOURCE_UART1_TX);
    EVIC_SourceStatusClear(INT_SOURCE_UART1_RX);
    EVIC_SourceStatusClear(INT_SOURCE_UART1_TX);

    // Clear buffers
    Uart1ClearReadBuffer();
    Uart1ClearWriteBuffer();
}

/**
 * @brief Returns the number of bytes available in the read buffer.
 * @return Number of bytes available in the read buffer.
 */
size_t Uart1GetReadAvailable() {

    // Trigger RX interrupt if hardware receive buffer not empty
    if (U1STAbits.URXDA == 1) {
        EVIC_SourceEnable(INT_SOURCE_UART1_RX);
        EVIC_SourceStatusSet(INT_SOURCE_UART1_RX);
    }

    // Clear hardware receive buffer overrun flag
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
        receiveBufferOverrun = true;
    }

    // Return number of bytes
    return CircularBufferGetReadAvailable(&readBuffer);
}

/**
 * @brief Reads data from the read buffer.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes.
 * @return Number of bytes read.
 */
size_t Uart1Read(void* const destination, size_t numberOfBytes) {
    Uart1GetReadAvailable(); // process hardware receive buffer
    return CircularBufferRead(&readBuffer, destination, numberOfBytes);
}

/**
 * @brief Reads a byte from the read buffer.  This function must only be called
 * if there are bytes available in the read buffer.
 * @return Byte.
 */
uint8_t Uart1ReadByte() {
    return CircularBufferReadByte(&readBuffer);
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t Uart1GetWriteAvailable() {
    return CircularBufferGetWriteAvailable(&writeBuffer);
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void Uart1Write(const void* const data, const size_t numberOfBytes) {
    CircularBufferWrite(&writeBuffer, data, numberOfBytes);
    EVIC_SourceEnable(INT_SOURCE_UART1_TX);
}

/**
 * @brief Writes a byte to the write buffer.
 * @param byte Byte.
 */
void Uart1WriteByte(const uint8_t byte) {
    CircularBufferWriteByte(&writeBuffer, byte);
    EVIC_SourceEnable(INT_SOURCE_UART1_TX);
}

/**
 * @brief Clears the read buffer and resets the read buffer overrun flag.
 */
void Uart1ClearReadBuffer() {
    CircularBufferClear(&readBuffer);
    Uart1HasReceiveBufferOverrun();
}

/**
 * @brief Clears the write buffer.
 */
void Uart1ClearWriteBuffer() {
    CircularBufferClear(&writeBuffer);
}

/**
 * @brief Returns true if the hardware receive buffer has overrun. Calling this
 * function will reset the flag.
 * @return True if the hardware receive buffer has overrun.
 */
bool Uart1HasReceiveBufferOverrun() {
    if (receiveBufferOverrun == true) {
        receiveBufferOverrun = false;
        return true;
    }
    return false;
}

/**
 * @brief Returns true if all data has been transmitted.
 * @return True if all data has been transmitted.
 */
bool Uart1IsTransmitionComplete() {
    return (EVIC_SourceIsEnabled(INT_SOURCE_UART1_TX) == false) && (U1STAbits.TRMT == 1);
}

#ifdef _UART_1_VECTOR

/**
 * @brief UART RX and TX interrupt handler.  This function should be called by
 * the ISR implementation generated by MPLAB Harmony.
 */
void Uart1InterruptHandler() {

    // RX interrupt
    if (EVIC_SourceStatusGet(INT_SOURCE_UART1_RX) == true) {
        RXInterruptTasks();
    }

    // TX interrupt
    if (EVIC_SourceIsEnabled(INT_SOURCE_UART1_TX) == false) {
        return; // return if TX interrupt disabled because TX interrupt flag will remain set while the transmit buffer is empty
    }
    if (EVIC_SourceStatusGet(INT_SOURCE_UART1_TX) == true) {
        TXInterruptTasks();
    }
}

#else

/**
 * @brief UART RX interrupt handler.  This function should be called by the
 * ISR implementation generated by MPLAB Harmony.
 */
void Uart1RXInterruptHandler() {
    RXInterruptTasks();
}

/**
 * @brief UART TX interrupt handler.  This function should be called by the
 * ISR implementation generated by MPLAB Harmony.
 */
void Uart1TXInterruptHandler() {
    TXInterruptTasks();
}

#endif

/**
 * @brief UART RX interrupt tasks.
 */
static inline __attribute__((always_inline)) void RXInterruptTasks() {
    while (U1STAbits.URXDA == 1) { // repeat while data available in receive buffer
        if (CircularBufferGetWriteAvailable(&readBuffer) == 0) { // if read buffer full
            EVIC_SourceDisable(INT_SOURCE_UART1_RX);
            break;
        } else {
            CircularBufferWriteByte(&readBuffer, U1RXREG);
        }
    }
    EVIC_SourceStatusClear(INT_SOURCE_UART1_RX);
}

/**
 * @brief UART TX interrupt tasks.
 */
static inline __attribute__((always_inline)) void TXInterruptTasks() {
    EVIC_SourceDisable(INT_SOURCE_UART1_TX); // disable TX interrupt to avoid nested interrupt
    EVIC_SourceStatusClear(INT_SOURCE_UART1_TX);
    while (U1STAbits.UTXBF == 0) { // repeat while transmit buffer not full
        if (CircularBufferGetReadAvailable(&writeBuffer) == 0) { // if write buffer empty
            return;
        }
        U1TXREG = CircularBufferReadByte(&writeBuffer);
    }
    EVIC_SourceEnable(INT_SOURCE_UART1_TX); // re-enable TX interrupt
}

//------------------------------------------------------------------------------
// End of file
