/**
 * @file Uart2DmaTx.c
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for TX only.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "definitions.h"
#include "Fifo.h"
#include <stdint.h>
#include "sys/kmem.h"
#include "Uart2DmaTx.h"

//------------------------------------------------------------------------------
// Function declarations

static inline __attribute__((always_inline)) void RxInterruptTasks(void);

//------------------------------------------------------------------------------
// Variables

static bool receiveBufferOverrun;
static uint8_t readData[UART2_READ_BUFFER_SIZE];
static Fifo readFifo = {.data = readData, .dataSize = sizeof (readData)};
static void (*writeComplete)(void);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 */
void Uart2DmaTxInitialise(const UartSettings * const settings) {

    // Ensure default register states
    Uart2DmaTxDeinitialise();

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
    U2STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U2STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U2BRG = UartCalculateUxbrg(settings->baudRate);
    U2MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

    // Enable DMA
    DMACONbits.ON = 1;

    // Configure TX DMA channel
    DCH0ECONbits.CHSIRQ = _UART2_TX_VECTOR;
    DCH0ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH0DSA = KVA_TO_PA(&U2TXREG); // destination address
    DCH0DSIZ = 1; // destination size
    DCH0CSIZ = 1; // transfers per event
    DCH0INTbits.CHBCIE = 1; // channel Block Transfer Complete Interrupt Enable bit

    // Enable interrupts
    EVIC_SourceEnable(INT_SOURCE_UART2_RX);
    EVIC_SourceEnable(INT_SOURCE_DMA0);
}

/**
 * @brief Deinitialises the module.
 */
void Uart2DmaTxDeinitialise(void) {

    // Disable UART and restore default register states
    U2MODE = 0;
    U2STA = 0;

    // Disable TX DMA channel and restore default register states
    DCH0CON = 0;
    DCH0ECON = 0;
    DCH0INT = 0;
    DCH0SSA = 0;
    DCH0DSA = 0;
    DCH0SSIZ = 0;
    DCH0DSIZ = 0;
    DCH0SPTR = 0;
    DCH0DPTR = 0;
    DCH0CSIZ = 0;
    DCH0CPTR = 0;
    DCH0DAT = 0;

    // Disable interrupts
    EVIC_SourceDisable(INT_SOURCE_UART2_RX);
    EVIC_SourceDisable(INT_SOURCE_DMA0);
    EVIC_SourceStatusClear(INT_SOURCE_UART2_RX);
    EVIC_SourceStatusClear(INT_SOURCE_DMA0);

    // Clear buffer
    Uart2DmaTxClearReadBuffer();
}

/**
 * @brief Returns the number of bytes available in the read buffer.
 * @return Number of bytes available in the read buffer.
 */
size_t Uart2DmaTxAvailableRead(void) {

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
    return FifoAvailableRead(&readFifo);
}

/**
 * @brief Reads data from the read buffer.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes.
 * @return Number of bytes read.
 */
size_t Uart2DmaTxRead(void* const destination, size_t numberOfBytes) {
    Uart2DmaTxAvailableRead(); // process hardware receive buffer
    return FifoRead(&readFifo, destination, numberOfBytes);
}

/**
 * @brief Reads a byte from the read buffer. This function must only be called
 * if there are bytes available in the read buffer.
 * @return Byte.
 */
uint8_t Uart2DmaTxReadByte(void) {
    return FifoReadByte(&readFifo);
}

/**
 * @brief Writes data. The data must be declared __attribute__((coherent)) for
 * PIC32MZ devices. This function must not be called while a write is in
 * progress.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @param writeComplete_ Write complete callback.
 */
void Uart2DmaTxWrite(const void* const data, const size_t numberOfBytes, void (*const writeComplete_) (void)) {
    writeComplete = writeComplete_;
    DCH0SSA = KVA_TO_PA(data); // source address
    DCH0SSIZ = numberOfBytes; // source size
    DCH0INTbits.CHBCIF = 0; // clear TX DMA channel interrupt flag
    DCH0CONbits.CHEN = 1; // enable TX DMA channel to begin write
}

/**
 * @brief DMA interrupt handler. This function should be called by the ISR
 * implementation generated by MPLAB Harmony.
 */
void Dma0InterruptHandler(void) {
    EVIC_SourceStatusClear(INT_SOURCE_DMA0); // clear interrupt flag first because callback may start new write
    if (writeComplete != NULL) {
        writeComplete();
    }
}

/**
 * @brief Returns true while data is being transferred to the hardware transmit
 * buffer.
 * @return True while data is being transferred to the hardware transmit buffer.
 */
bool Uart2DmaTxWriteInProgress(void) {
    return DCH0CONbits.CHEN == 1;
}

/**
 * @brief Clears the read buffer and resets the read buffer overrun flag.
 */
void Uart2DmaTxClearReadBuffer(void) {
    FifoClear(&readFifo);
    Uart2DmaTxReceiveBufferOverrun();
}

/**
 * @brief Returns true if the hardware receive buffer has overrun. Calling this
 * function will reset the flag.
 * @return True if the hardware receive buffer has overrun.
 */
bool Uart2DmaTxReceiveBufferOverrun(void) {
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
bool Uart2DmaTxTransmitionComplete(void) {
    return (Uart2DmaTxWriteInProgress() == false) && (U2STAbits.TRMT == 1);
}

#ifdef _UART_2_VECTOR

/**
 * @brief UART RX and TX interrupt handler. This function should be called by
 * the ISR implementation generated by MPLAB Harmony.
 */
void Uart2DmaTxInterruptHandler(void) {
    if (EVIC_SourceStatusGet(INT_SOURCE_UART2_RX)) {
        RxInterruptTasks();
    }
}

#else

/**
 * @brief UART RX interrupt handler. This function should be called by the ISR
 * implementation generated by MPLAB Harmony.
 */
void Uart2RxInterruptHandler(void) {
    RxInterruptTasks();
}

#endif

/**
 * @brief UART RX interrupt tasks.
 */
static inline __attribute__((always_inline)) void RxInterruptTasks(void) {
    while (U2STAbits.URXDA == 1) { // while data available in receive buffer
        if (FifoAvailableWrite(&readFifo) == 0) { // if read buffer full
            EVIC_SourceDisable(INT_SOURCE_UART2_RX);
            break;
        } else {
            FifoWriteByte(&readFifo, U2RXREG);
        }
    }
    EVIC_SourceStatusClear(INT_SOURCE_UART2_RX);
}

//------------------------------------------------------------------------------
// End of file
