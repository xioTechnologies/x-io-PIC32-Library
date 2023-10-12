/**
 * @file Uart3.c
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "CircularBuffer.h"
#include "definitions.h"
#include <stdint.h>
#include "Uart3.h"

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
void Uart3Initialise(const UartSettings * const settings) {

    // Ensure default register states
    Uart3Deinitialise();

    // Configure UART
    if (settings->rtsCtsEnabled == true) {
        U3MODEbits.UEN = 0b10; // UxTX, UxRX, UxCTS and UxRTS pins are enabled and used
    }
    if (settings->invertTXRX == true) {
        U3MODEbits.RXINV = 1; // UxRX Idle state is '0'
        U3STAbits.UTXINV = 1; // UxTX Idle state is '0'
    }
    U3MODEbits.PDSEL = settings->parityAndData;
    U3MODEbits.STSEL = settings->stopBits;
    U3MODEbits.BRGH = 1; // High-Speed mode - 4x baud clock enabled
    U3STAbits.URXISEL = 0b01; // Interrupt flag bit is asserted while receive buffer is 1/2 or more full (i.e., has 4 or more data characters)
    U3STAbits.UTXISEL = 0b10; // Interrupt is generated and asserted while the transmit buffer is empty
    U3STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U3STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U3BRG = UartCalculateUxbrg(settings->baudRate);
    U3MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

    // Configure interrupts
    EVIC_SourceEnable(INT_SOURCE_UART3_RX); // enable RX interrupt only
}

/**
 * @brief Deinitialises the module.
 */
void Uart3Deinitialise() {

    // Disable UART and restore default register states
    U3MODE = 0;
    U3STA = 0;

    // Disable interrupts
    EVIC_SourceDisable(INT_SOURCE_UART3_RX);
    EVIC_SourceDisable(INT_SOURCE_UART3_TX);
    EVIC_SourceStatusClear(INT_SOURCE_UART3_RX);
    EVIC_SourceStatusClear(INT_SOURCE_UART3_TX);

    // Clear buffers
    Uart3ClearReadBuffer();
    Uart3ClearWriteBuffer();
}

/**
 * @brief Returns the number of bytes available in the read buffer.
 * @return Number of bytes available in the read buffer.
 */
size_t Uart3GetReadAvailable() {

    // Trigger RX interrupt if hardware receive buffer not empty
    if (U3STAbits.URXDA == 1) {
        EVIC_SourceEnable(INT_SOURCE_UART3_RX);
        EVIC_SourceStatusSet(INT_SOURCE_UART3_RX);
    }

    // Clear hardware receive buffer overrun flag
    if (U3STAbits.OERR == 1) {
        U3STAbits.OERR = 0;
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
size_t Uart3Read(void* const destination, size_t numberOfBytes) {
    Uart3GetReadAvailable(); // process hardware receive buffer
    return CircularBufferRead(&readBuffer, destination, numberOfBytes);
}

/**
 * @brief Reads a byte from the read buffer.  This function must only be called
 * if there are bytes available in the read buffer.
 * @return Byte.
 */
uint8_t Uart3ReadByte() {
    return CircularBufferReadByte(&readBuffer);
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t Uart3GetWriteAvailable() {
    return CircularBufferGetWriteAvailable(&writeBuffer);
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void Uart3Write(const void* const data, const size_t numberOfBytes) {
    CircularBufferWrite(&writeBuffer, data, numberOfBytes);
    EVIC_SourceEnable(INT_SOURCE_UART3_TX);
}

/**
 * @brief Writes a byte to the write buffer.
 * @param byte Byte.
 */
void Uart3WriteByte(const uint8_t byte) {
    CircularBufferWriteByte(&writeBuffer, byte);
    EVIC_SourceEnable(INT_SOURCE_UART3_TX);
}

/**
 * @brief Clears the read buffer and resets the read buffer overrun flag.
 */
void Uart3ClearReadBuffer() {
    CircularBufferClear(&readBuffer);
    Uart3HasReceiveBufferOverrun();
}

/**
 * @brief Clears the write buffer.
 */
void Uart3ClearWriteBuffer() {
    CircularBufferClear(&writeBuffer);
}

/**
 * @brief Returns true if the hardware receive buffer has overrun. Calling this
 * function will reset the flag.
 * @return True if the hardware receive buffer has overrun.
 */
bool Uart3HasReceiveBufferOverrun() {
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
bool Uart3IsTransmitionComplete() {
    return (EVIC_SourceIsEnabled(INT_SOURCE_UART3_TX) == false) && (U3STAbits.TRMT == 1);
}

#ifdef _UART_3_VECTOR

/**
 * @brief UART RX and TX interrupt handler.  This function should be called by
 * the ISR implementation generated by MPLAB Harmony.
 */
void Uart3InterruptHandler() {

    // RX interrupt
    if (EVIC_SourceStatusGet(INT_SOURCE_UART3_RX) == true) {
        RXInterruptTasks();
    }

    // TX interrupt
    if (EVIC_SourceIsEnabled(INT_SOURCE_UART3_TX) == false) {
        return; // return if TX interrupt disabled because TX interrupt flag will remain set while the transmit buffer is empty
    }
    if (EVIC_SourceStatusGet(INT_SOURCE_UART3_TX) == true) {
        TXInterruptTasks();
    }
}

#else

/**
 * @brief UART RX interrupt handler.  This function should be called by the
 * ISR implementation generated by MPLAB Harmony.
 */
void Uart3RXInterruptHandler() {
    RXInterruptTasks();
}

/**
 * @brief UART TX interrupt handler.  This function should be called by the
 * ISR implementation generated by MPLAB Harmony.
 */
void Uart3TXInterruptHandler() {
    TXInterruptTasks();
}

#endif

/**
 * @brief UART RX interrupt tasks.
 */
static inline __attribute__((always_inline)) void RXInterruptTasks() {
    while (U3STAbits.URXDA == 1) { // repeat while data available in receive buffer
        if (CircularBufferGetWriteAvailable(&readBuffer) == 0) { // if read buffer full
            EVIC_SourceDisable(INT_SOURCE_UART3_RX);
            break;
        } else {
            CircularBufferWriteByte(&readBuffer, U3RXREG);
        }
    }
    EVIC_SourceStatusClear(INT_SOURCE_UART3_RX);
}

/**
 * @brief UART TX interrupt tasks.
 */
static inline __attribute__((always_inline)) void TXInterruptTasks() {
    EVIC_SourceDisable(INT_SOURCE_UART3_TX); // disable TX interrupt to avoid nested interrupt
    EVIC_SourceStatusClear(INT_SOURCE_UART3_TX);
    while (U3STAbits.UTXBF == 0) { // repeat while transmit buffer not full
        if (CircularBufferGetReadAvailable(&writeBuffer) == 0) { // if write buffer empty
            return;
        }
        U3TXREG = CircularBufferReadByte(&writeBuffer);
    }
    EVIC_SourceEnable(INT_SOURCE_UART3_TX); // re-enable TX interrupt
}

//------------------------------------------------------------------------------
// End of file
