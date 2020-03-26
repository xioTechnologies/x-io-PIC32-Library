/**
 * @file Uart6.c
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "CircularBuffer.h"
#include <stdint.h>
#include <string.h> // strlen
#include "system_definitions.h"
#include "Uart6.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Read and write buffers size.  Must be a 2^n number (e.g. 256, 512,
 * 1024, 2048, 4096, etc.).
 */
#define BUFFER_SIZE (4096)

/**
 * @brief Read and write buffers index mask.  This value is bitwise anded with
 * buffer indexes for fast overflow operations.
 */
#define BUFFER_INDEX_BIT_MASK (BUFFER_SIZE - 1)

/**
 * @brief TX and RX interrupt priority.
 */
#define INTERRUPT_PRIORITY (INT_PRIORITY_LEVEL4)

//------------------------------------------------------------------------------
// Function declarations

static inline __attribute__((always_inline)) void RXInterruptTasks();
static inline __attribute__((always_inline)) void TXInterruptTasks();

//------------------------------------------------------------------------------
// Variables

static bool receiveBufferOverrun;
static uint8_t readBuffer[BUFFER_SIZE];
static int readBufferWriteIndex;
static int readBufferReadIndex;
static uint8_t writeBuffer[BUFFER_SIZE];
static int writeBufferWriteIndex;
static int writeBufferReadIndex;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 */
void Uart6Initialise(const UartSettings * const settings) {

    // Ensure default register states
    Uart6Disable();

    // Configure UART
    if (settings->rtsCtsEnabled == true) {
        U6MODEbits.UEN = 0b10; // UxTX, UxRX, UxCTS and UxRTS pins are enabled and used
    }
    if (settings->invertDataLines == true) {
        U6MODEbits.RXINV = 1; // UxRX Idle state is '0'
        U6STAbits.UTXINV = 1; // UxTX Idle state is '0'
    }
    U6MODEbits.PDSEL = settings->parityAndData;
    U6MODEbits.STSEL = settings->stopBits;
    U6MODEbits.BRGH = 1; // High-Speed mode - 4x baud clock enabled
    U6STAbits.URXISEL = 0b01; // Interrupt flag bit is asserted while receive buffer is 1/2 or more full (i.e., has 4 or more data characters)
    U6STAbits.UTXISEL = 0b10; // Interrupt is generated and asserted while the transmit buffer is empty
    U6STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U6STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U6BRG = UartCalculateUxbrg(settings->baudRate);
    U6MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

    // Configure interrupts
#ifdef _UART_6_VECTOR
    SYS_INT_VectorPrioritySet(INT_VECTOR_UART6, INTERRUPT_PRIORITY);
#else
    SYS_INT_VectorPrioritySet(INT_VECTOR_UART6_RX, INTERRUPT_PRIORITY);
    SYS_INT_VectorPrioritySet(INT_VECTOR_UART6_TX, INTERRUPT_PRIORITY);
#endif
    SYS_INT_SourceEnable(INT_SOURCE_USART_6_RECEIVE); // enable RX interrupt only
}

/**
 * @brief Disables the module.
 */
void Uart6Disable() {

    // Disable UART and restore default register states
    U6MODE = 0;
    U6STA = 0;

    // Disable interrupts
    SYS_INT_SourceDisable(INT_SOURCE_USART_6_RECEIVE);
    SYS_INT_SourceDisable(INT_SOURCE_USART_6_TRANSMIT);
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_6_RECEIVE);
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_6_TRANSMIT);

    // Clear buffers
    Uart6ClearReadBuffer();
    Uart6ClearWriteBuffer();
}

/**
 * @brief Returns the number of bytes available in the read buffer.
 * @return Number of bytes available in the read buffer.
 */
size_t Uart6GetReadAvailable() {

    // Trigger RX interrupt if hardware receive buffer not empty
    if (U6STAbits.URXDA == 1) {
        SYS_INT_SourceEnable(INT_SOURCE_USART_6_RECEIVE);
        SYS_INT_SourceStatusSet(INT_SOURCE_USART_6_RECEIVE);
    }

    // Clear hardware receive buffer overrun flag
    if (U6STAbits.OERR == 1) {
        U6STAbits.OERR = 0;
        receiveBufferOverrun = true;
    }

    // Return number of bytes
    return (readBufferWriteIndex - readBufferReadIndex) & BUFFER_INDEX_BIT_MASK;
}

/**
 * @brief Reads data from the read buffer.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes.
 * @return Number of bytes read.
 */
size_t Uart6Read(void* const destination, size_t numberOfBytes) {

    // Limit number of bytes to number available
    const size_t bytesAvailable = Uart6GetReadAvailable();
    if (numberOfBytes > bytesAvailable) {
        numberOfBytes = bytesAvailable;
    }

    // Read data
    readBufferReadIndex &= BUFFER_INDEX_BIT_MASK;
    CircularBufferRead(readBuffer, BUFFER_SIZE, &readBufferReadIndex, destination, numberOfBytes);
    return numberOfBytes;
}

/**
 * @brief Reads a byte from the read buffer.  This function must only be called
 * if there are bytes available in the read buffer.
 * @return Byte.
 */
uint8_t Uart6ReadByte() {
    return readBuffer[readBufferReadIndex++ & BUFFER_INDEX_BIT_MASK];
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t Uart6GetWriteAvailable() {
    return (BUFFER_SIZE - 1) - ((writeBufferWriteIndex - writeBufferReadIndex) & BUFFER_INDEX_BIT_MASK);
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void Uart6Write(const void* const data, const size_t numberOfBytes) {

    // Do nothing if not enough space available
    if (Uart6GetWriteAvailable() < numberOfBytes) {
        return;
    }

    // Write data
    writeBufferWriteIndex &= BUFFER_INDEX_BIT_MASK;
    CircularBufferWrite(writeBuffer, BUFFER_SIZE, &writeBufferWriteIndex, data, numberOfBytes);
    SYS_INT_SourceEnable(INT_SOURCE_USART_6_TRANSMIT);
}

/**
 * @brief Writes a byte to the write buffer.
 * @param byte Byte.
 */
void Uart6WriteByte(const uint8_t byte) {

    // Do nothing if not enough space available
    if (Uart6GetWriteAvailable() == 0) {
        return;
    }

    // Write byte
    writeBuffer[writeBufferWriteIndex++ & BUFFER_INDEX_BIT_MASK] = byte;
    SYS_INT_SourceEnable(INT_SOURCE_USART_6_TRANSMIT);
}

/**
 * @brief Writes a string to the write buffer.
 * @param string String.
 */
void Uart6WriteString(const char* string) {

    // Do nothing if not enough space available
    if (Uart6GetWriteAvailable() < strlen(string)) {
        return;
    }

    // Write string
    while (*string != '\0') {
        writeBuffer[writeBufferWriteIndex++ & BUFFER_INDEX_BIT_MASK] = *string++;
    }
    SYS_INT_SourceEnable(INT_SOURCE_USART_6_TRANSMIT);
}

/**
 * @brief Clears the read buffer and resets the read buffer overrun flag.
 */
void Uart6ClearReadBuffer() {
    readBufferReadIndex = readBufferWriteIndex & BUFFER_INDEX_BIT_MASK;
    Uart6HasReceiveBufferOverrun();
}

/**
 * @brief Clears the write buffer.
 */
void Uart6ClearWriteBuffer() {
    writeBufferWriteIndex = writeBufferReadIndex & BUFFER_INDEX_BIT_MASK;
}

/**
 * @brief Returns true if the hardware receive buffer has overrun. Calling this
 * function will reset the flag.
 * @return True if the hardware receive buffer has overrun.
 */
bool Uart6HasReceiveBufferOverrun() {
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
bool Uart6IsTransmitionComplete() {
    return (SYS_INT_SourceIsEnabled(INT_SOURCE_USART_6_TRANSMIT) == false) && (U6STAbits.TRMT == 1);
}

#ifdef _UART_6_VECTOR

/**
 * @brief UART RX and TX interrupt.
 */
void __ISR(_UART_6_VECTOR) Uart6Interrupt() {

    // RX interrupt
    if (SYS_INT_SourceStatusGet(INT_SOURCE_USART_6_RECEIVE) == true) {
        RXInterruptTasks();
    }

    // TX interrupt
    if (SYS_INT_SourceIsEnabled(INT_SOURCE_USART_6_TRANSMIT) == false) {
        return; // return if TX interrupt disabled because TX interrupt flag will remain set while the transmit buffer is empty
    }
    if (SYS_INT_SourceStatusGet(INT_SOURCE_USART_6_TRANSMIT) == true) {
        TXInterruptTasks();
    }
}

#else

/**
 * @brief UART RX interrupt.
 */
void __ISR(_UART6_RX_VECTOR) Uart6RxInterrupt() {
    RXInterruptTasks();
}

/**
 * @brief UART TX interrupt.
 */
void __ISR(_UART6_TX_VECTOR) Uart6TxInterrupt() {
    TXInterruptTasks();
}

#endif

/**
 * @brief UART RX interrupt tasks.
 */
static inline __attribute__((always_inline)) void RXInterruptTasks() {
    while (U6STAbits.URXDA == 1) { // repeat while data available in receive buffer
        if (((readBufferReadIndex - readBufferWriteIndex) & BUFFER_INDEX_BIT_MASK) == 1) { // if read buffer full
            SYS_INT_SourceDisable(INT_SOURCE_USART_6_RECEIVE);
            break;
        } else {
            readBuffer[readBufferWriteIndex++ & BUFFER_INDEX_BIT_MASK] = U6RXREG;
        }
    }
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_6_RECEIVE);
}

/**
 * @brief UART TX interrupt tasks.
 */
static inline __attribute__((always_inline)) void TXInterruptTasks() {
    SYS_INT_SourceDisable(INT_SOURCE_USART_6_TRANSMIT); // disable TX interrupt to avoid nested interrupt
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_6_TRANSMIT);
    while (U6STAbits.UTXBF == 0) { // repeat while transmit buffer not full
        if (((writeBufferReadIndex - writeBufferWriteIndex) & BUFFER_INDEX_BIT_MASK) == 0) { // if write buffer empty
            return;
        }
        U6TXREG = writeBuffer[writeBufferReadIndex++ & BUFFER_INDEX_BIT_MASK];
    }
    SYS_INT_SourceEnable(INT_SOURCE_USART_6_TRANSMIT); // re-enable TX interrupt
}

//------------------------------------------------------------------------------
// End of file
