/**
 * @file Uart4DmaTX.c
 * @author Seb Madgwick
 * @brief UART driver for PIC32 devices. DMA used for TX only.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Fifo.h"
#include <stdint.h>
#include "sys/kmem.h"
#include "Uart4DmaTX.h"

//------------------------------------------------------------------------------
// Function declarations

static inline __attribute__((always_inline)) void RXInterruptTasks(void);

//------------------------------------------------------------------------------
// Variables

static bool receiveBufferOverrun;
static uint8_t readData[4096];
static Fifo readFifo = {.data = readData, .dataSize = sizeof (readData)};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 */
void Uart4DmaTXInitialise(const UartSettings * const settings) {

    // Ensure default register states
    Uart4DmaTXDeinitialise();

    // Configure UART
    if (settings->rtsCtsEnabled) {
        U4MODEbits.UEN = 0b10; // UxTX, UxRX, UxCTS and UxRTS pins are enabled and used
    }
    if (settings->invertTXRX) {
        U4MODEbits.RXINV = 1; // UxRX Idle state is '0'
        U4STAbits.UTXINV = 1; // UxTX Idle state is '0'
    }
    U4MODEbits.PDSEL = settings->parityAndData;
    U4MODEbits.STSEL = settings->stopBits;
    U4MODEbits.BRGH = 1; // High-Speed mode - 4x baud clock enabled
    U4STAbits.URXISEL = 0b01; // Interrupt flag bit is asserted while receive buffer is 1/2 or more full (i.e., has 4 or more data characters)
    U4STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U4STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U4BRG = UartCalculateUxbrg(settings->baudRate);
    U4MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

    // Enable DMA
    DMACONbits.ON = 1;

    // Configure TX DMA channel
    DCH0ECONbits.CHSIRQ = _UART4_TX_VECTOR;
    DCH0ECONbits.SIRQEN = 1; // Start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH0DSA = KVA_TO_PA(&U4TXREG); // destination address
    DCH0DSIZ = 1; // destination size
    DCH0CSIZ = 1; // transfers per event

    // Configure interrupts
    EVIC_SourceEnable(INT_SOURCE_UART4_RX); // enable RX interrupt only
}

/**
 * @brief Deinitialises the module.
 */
void Uart4DmaTXDeinitialise(void) {

    // Disable UART and restore default register states
    U4MODE = 0;
    U4STA = 0;

    // Disable interrupt
    EVIC_SourceDisable(INT_SOURCE_UART4_RX);
    EVIC_SourceStatusClear(INT_SOURCE_UART4_RX);

    // Clear buffer
    Uart4DmaTXClearReadBuffer();
}

/**
 * @brief Returns the number of bytes available in the read buffer.
 * @return Number of bytes available in the read buffer.
 */
size_t Uart4DmaTXGetReadAvailable(void) {

    // Trigger RX interrupt if hardware receive buffer not empty
    if (U4STAbits.URXDA == 1) {
        EVIC_SourceEnable(INT_SOURCE_UART4_RX);
        EVIC_SourceStatusSet(INT_SOURCE_UART4_RX);
    }

    // Clear hardware receive buffer overrun flag
    if (U4STAbits.OERR == 1) {
        U4STAbits.OERR = 0;
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
size_t Uart4DmaTXRead(void* const destination, size_t numberOfBytes) {
    Uart4DmaTXGetReadAvailable(); // process hardware receive buffer
    return FifoRead(&readFifo, destination, numberOfBytes);
}

/**
 * @brief Reads a byte from the read buffer. This function must only be called
 * if there are bytes available in the read buffer.
 * @return Byte.
 */
uint8_t Uart4DmaTXReadByte(void) {
    return FifoReadByte(&readFifo);
}

/**
 * @brief Writes data. The data must be declared __attribute__((coherent)) for
 * PIC32MZ devices. This function must not be called while a write is in
 * progress.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void Uart4DmaTXWrite(const void* const data, const size_t numberOfBytes) {
    DCH0SSA = KVA_TO_PA(data); // source address
    DCH0SSIZ = numberOfBytes; // source size
    DCH0CONbits.CHEN = 1; // enable TX DMA channel
}

/**
 * @brief Returns true while data is being transferred to the hardware transmit
 * buffer.
 * @return True while data is being transferred to the hardware transmit buffer.
 */
bool Uart4DmaTXWriteInProgress(void) {
    return DCH0CONbits.CHEN == 1;
}

/**
 * @brief Clears the read buffer and resets the read buffer overrun flag.
 */
void Uart4DmaTXClearReadBuffer(void) {
    FifoClear(&readFifo);
    Uart4DmaTXHasReceiveBufferOverrun();
}

/**
 * @brief Returns true if the hardware receive buffer has overrun. Calling this
 * function will reset the flag.
 * @return True if the hardware receive buffer has overrun.
 */
bool Uart4DmaTXHasReceiveBufferOverrun(void) {
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
bool Uart4DmaTXTransmitionComplete(void) {
    return (Uart4DmaTXWriteInProgress() == false) && (U4STAbits.TRMT == 1);
}

#ifdef _UART_4_VECTOR

/**
 * @brief UART RX and TX interrupt handler. This function should be called by
 * the ISR implementation generated by MPLAB Harmony.
 */
void Uart4DmaTXInterruptHandler(void) {
    if (EVIC_SourceStatusGet(INT_SOURCE_UART4_RX)) {
        RXInterruptTasks();
    }
}

#else

/**
 * @brief UART RX interrupt handler. This function should be called by the
 * ISR implementation generated by MPLAB Harmony.
 */
void Uart4RXInterruptHandler(void) {
    RXInterruptTasks();
}

#endif

/**
 * @brief UART RX interrupt tasks.
 */
static inline __attribute__((always_inline)) void RXInterruptTasks(void) {
    while (U4STAbits.URXDA == 1) { // repeat while data available in receive buffer
        if (FifoGetWriteAvailable(&readFifo) == 0) { // if read buffer full
            EVIC_SourceDisable(INT_SOURCE_UART4_RX);
            break;
        } else {
            FifoWriteByte(&readFifo, U4RXREG);
        }
    }
    EVIC_SourceStatusClear(INT_SOURCE_UART4_RX);
}

//------------------------------------------------------------------------------
// End of file
