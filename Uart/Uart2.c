/**
 * @file Uart2.c
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Fifo.h"
#include <stdint.h>
#include "Uart2.h"

//------------------------------------------------------------------------------
// Function declarations

static inline __attribute__((always_inline)) void RXInterruptTasks(void);
static inline __attribute__((always_inline)) void TXInterruptTasks(void);

//------------------------------------------------------------------------------
// Variables

static bool receiveBufferOverrun;
static uint8_t readData[4096];
static Fifo readFifo = {.data = readData, .dataSize = sizeof (readData)};
static uint8_t writeData[4096];
static Fifo writeFifo = {.data = writeData, .dataSize = sizeof (writeData)};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 */
void Uart2Initialise(const UartSettings * const settings) {

    // Ensure default register states
    Uart2Deinitialise();

    // Configure UART
    if (settings->rtsCtsEnabled) {
        U2MODEbits.UEN = 0b10; // UxTX, UxRX, UxCTS and UxRTS pins are enabled and used
    }
    if (settings->invertTXRX) {
        U2MODEbits.RXINV = 1; // UxRX Idle state is '0'
        U2STAbits.UTXINV = 1; // UxTX Idle state is '0'
    }
    U2MODEbits.PDSEL = settings->parityAndData;
    U2MODEbits.STSEL = settings->stopBits;
    U2MODEbits.BRGH = 1; // high-Speed mode - 4x baud clock enabled
    U2STAbits.URXISEL = 0b01; // interrupt flag bit is asserted while receive buffer is 1/2 or more full (i.e., has 4 or more data characters)
    U2STAbits.UTXISEL = 0b10; // interrupt is generated and asserted while the transmit buffer is empty
    U2STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U2STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U2BRG = UartCalculateUxbrg(settings->baudRate);
    U2MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

    // Configure interrupts
    EVIC_SourceEnable(INT_SOURCE_UART2_RX); // enable RX interrupt only
}

/**
 * @brief Deinitialises the module.
 */
void Uart2Deinitialise(void) {

    // Disable UART and restore default register states
    U2MODE = 0;
    U2STA = 0;

    // Disable interrupts
    EVIC_SourceDisable(INT_SOURCE_UART2_RX);
    EVIC_SourceDisable(INT_SOURCE_UART2_TX);
    EVIC_SourceStatusClear(INT_SOURCE_UART2_RX);
    EVIC_SourceStatusClear(INT_SOURCE_UART2_TX);

    // Clear buffers
    Uart2ClearReadBuffer();
    Uart2ClearWriteBuffer();
}

/**
 * @brief Returns the number of bytes available in the read buffer.
 * @return Number of bytes available in the read buffer.
 */
size_t Uart2GetReadAvailable(void) {

    // Trigger RX interrupt if hardware receive buffer not empty
    if (U2STAbits.URXDA == 1) {
        EVIC_SourceEnable(INT_SOURCE_UART2_RX);
        EVIC_SourceStatusSet(INT_SOURCE_UART2_RX);
    }

    // Clear hardware receive buffer overrun flag
    if (U2STAbits.OERR == 1) {
        U2STAbits.OERR = 0;
        receiveBufferOverrun = true;
    }

    // Return number of bytes
    return FifoGetReadAvailable(&readFifo);
}

/**
 * @brief Reads data from the read buffer.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes.
 * @return Number of bytes read.
 */
size_t Uart2Read(void* const destination, size_t numberOfBytes) {
    Uart2GetReadAvailable(); // process hardware receive buffer
    return FifoRead(&readFifo, destination, numberOfBytes);
}

/**
 * @brief Reads a byte from the read buffer. This function must only be called
 * if there are bytes available in the read buffer.
 * @return Byte.
 */
uint8_t Uart2ReadByte(void) {
    return FifoReadByte(&readFifo);
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t Uart2GetWriteAvailable(void) {
    return FifoGetWriteAvailable(&writeFifo);
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void Uart2Write(const void* const data, const size_t numberOfBytes) {
    FifoWrite(&writeFifo, data, numberOfBytes);
    EVIC_SourceEnable(INT_SOURCE_UART2_TX);
}

/**
 * @brief Writes a byte to the write buffer.
 * @param byte Byte.
 */
void Uart2WriteByte(const uint8_t byte) {
    FifoWriteByte(&writeFifo, byte);
    EVIC_SourceEnable(INT_SOURCE_UART2_TX);
}

/**
 * @brief Clears the read buffer and resets the read buffer overrun flag.
 */
void Uart2ClearReadBuffer(void) {
    FifoClear(&readFifo);
    Uart2HasReceiveBufferOverrun();
}

/**
 * @brief Clears the write buffer.
 */
void Uart2ClearWriteBuffer(void) {
    FifoClear(&writeFifo);
}

/**
 * @brief Returns true if the hardware receive buffer has overrun. Calling this
 * function will reset the flag.
 * @return True if the hardware receive buffer has overrun.
 */
bool Uart2HasReceiveBufferOverrun(void) {
    if (receiveBufferOverrun) {
        receiveBufferOverrun = false;
        return true;
    }
    return false;
}

/**
 * @brief Returns true if all data has been transmitted.
 * @return True if all data has been transmitted.
 */
bool Uart2TransmitionComplete(void) {
    return (EVIC_SourceIsEnabled(INT_SOURCE_UART2_TX) == false) && (U2STAbits.TRMT == 1);
}

#ifdef _UART_2_VECTOR

/**
 * @brief UART RX and TX interrupt handler. This function should be called by
 * the ISR implementation generated by MPLAB Harmony.
 */
void Uart2InterruptHandler(void) {

    // RX interrupt
    if (EVIC_SourceStatusGet(INT_SOURCE_UART2_RX)) {
        RXInterruptTasks();
    }

    // TX interrupt
    if (EVIC_SourceIsEnabled(INT_SOURCE_UART2_TX) == false) {
        return; // return if TX interrupt disabled because TX interrupt flag will remain set while the transmit buffer is empty
    }
    if (EVIC_SourceStatusGet(INT_SOURCE_UART2_TX)) {
        TXInterruptTasks();
    }
}

#else

/**
 * @brief UART RX interrupt handler. This function should be called by the
 * ISR implementation generated by MPLAB Harmony.
 */
void Uart2RXInterruptHandler(void) {
    RXInterruptTasks();
}

/**
 * @brief UART TX interrupt handler. This function should be called by the
 * ISR implementation generated by MPLAB Harmony.
 */
void Uart2TXInterruptHandler(void) {
    TXInterruptTasks();
}

#endif

/**
 * @brief UART RX interrupt tasks.
 */
static inline __attribute__((always_inline)) void RXInterruptTasks(void) {
    while (U2STAbits.URXDA == 1) { // repeat while data available in receive buffer
        if (FifoGetWriteAvailable(&readFifo) == 0) { // if read buffer full
            EVIC_SourceDisable(INT_SOURCE_UART2_RX);
            break;
        } else {
            FifoWriteByte(&readFifo, U2RXREG);
        }
    }
    EVIC_SourceStatusClear(INT_SOURCE_UART2_RX);
}

/**
 * @brief UART TX interrupt tasks.
 */
static inline __attribute__((always_inline)) void TXInterruptTasks(void) {
    EVIC_SourceDisable(INT_SOURCE_UART2_TX); // disable TX interrupt to avoid nested interrupt
    EVIC_SourceStatusClear(INT_SOURCE_UART2_TX);
    while (U2STAbits.UTXBF == 0) { // repeat while transmit buffer not full
        if (FifoGetReadAvailable(&writeFifo) == 0) { // if write buffer empty
            return;
        }
        U2TXREG = FifoReadByte(&writeFifo);
    }
    EVIC_SourceEnable(INT_SOURCE_UART2_TX); // re-enable TX interrupt
}

//------------------------------------------------------------------------------
// End of file
