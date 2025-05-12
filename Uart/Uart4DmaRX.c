/**
 * @file Uart4DmaRX.c
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for RX only.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include <stdint.h>
#include "sys/kmem.h"
#include "Timer/Timer.h"
#include "Uart4DmaRX.h"

//------------------------------------------------------------------------------
// Function declarations

static inline __attribute__((always_inline)) void BlockTransferComplete(void);
static inline __attribute__((always_inline)) void TransferAborted(void);
static inline __attribute__((always_inline)) void TXInterruptTasks(void);

//------------------------------------------------------------------------------
// Variables

static void (*read)(const void* const data, const size_t numberOfBytes);
static uint8_t __attribute__((coherent)) readBuffer[1024]; // must be declared __attribute__((coherent)) for PIC32MZ devices
static uint8_t writeData[4096];
static Fifo writeFifo = {.data = writeData, .dataSize = sizeof (writeData)};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 * @param readConditions Read conditions.
 * @param read Read callback.
 */
void Uart4DmaRXInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*read_)(const void* const data, const size_t numberOfBytes)) {

    // Ensure default register states
    Uart4DmaRXDeinitialise();

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
    U4MODEbits.BRGH = 1; // high-Speed mode - 4x baud clock enabled
    U4STAbits.UTXISEL = 0b10; // interrupt is generated and asserted while the transmit buffer is empty
    U4STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U4STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U4BRG = UartCalculateUxbrg(settings->baudRate);
    U4MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

    // Limit read condition values to valid range
    UartDmaReadConditions validReadConditions = *readConditions;
    if (validReadConditions.numberOfBytes > sizeof (readBuffer)) {
        validReadConditions.numberOfBytes = sizeof (readBuffer);
    }
    if (validReadConditions.terminationByte != -1) {
        validReadConditions.terminationByte &= 0xFF;
    }
    if (validReadConditions.timeout > 1000) {
        validReadConditions.timeout = 1000;
    }

    // Store callback
    read = read_;

    // Enable DMA
    DMACONbits.ON = 1;

    // Configure RX DMA channel
    DCH0ECONbits.CHAIRQ = _TIMER_9_VECTOR;
    DCH0ECONbits.CHSIRQ = _UART4_RX_VECTOR;
    DCH0ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH0ECONbits.AIRQEN = 1; // channel transfer is aborted if an interrupt matching CHAIRQ occurs
    if (readConditions->terminationByte != -1) {
        DCH0ECONbits.PATEN = 1; // abort transfer and clear CHEN on pattern match
    }
    DCH0SSA = KVA_TO_PA(&U4RXREG); // source address
    DCH0DSA = KVA_TO_PA(readBuffer); // destination address
    DCH0SSIZ = 1; // source size
    DCH0DSIZ = validReadConditions.numberOfBytes; // destination size
    DCH0CSIZ = 1; // transfers per event
    DCH0DAT = validReadConditions.terminationByte; // pattern data
    DCH0INTbits.CHBCIE = 1; // channel Block Transfer Complete Interrupt Enable bit
    DCH0INTbits.CHTAIE = 1; // channel Transfer Abort Interrupt Enable bit
    DCH0CONbits.CHEN = 1; // channel is enabled

    // Calculate timer reset value
    static uint32_t __attribute__((coherent)) timerResetValue[1]; // must be declared __attribute__((coherent)) for PIC32MZ devices
    timerResetValue[0] = UartDmaCalculateTimerResetValue(validReadConditions.timeout);

    // Configure timer DMA channel
    DCH1CONbits.CHAEN = 1; // channel is continuously enabled, and not automatically disabled after a block transfer is complete
    DCH1ECONbits.CHSIRQ = _UART4_RX_VECTOR;
    DCH1ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH1SSA = KVA_TO_PA(timerResetValue); // source address
    DCH1DSA = KVA_TO_PA(&TMR8); // destination address
    DCH1SSIZ = sizeof (timerResetValue); // source size
    DCH1DSIZ = sizeof (timerResetValue); // destination size
    DCH1CSIZ = sizeof (timerResetValue); // transfers per event
    DCH1CONbits.CHEN = 1; // channel is enabled

    // Configure timer
    T8CONbits.T32 = 1;
    T8CONbits.ON = 1;

    // Enable interrupts
    EVIC_SourceEnable(INT_SOURCE_DMA0);
}

/**
 * @brief Deinitialises the module.
 */
void Uart4DmaRXDeinitialise(void) {

    // Disable UART and restore default register states
    U4MODE = 0;
    U4STA = 0;

    // Disable RX DMA channel and restore default register states
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

    // Disable timer DMA channel and restore default register states
    DCH1CON = 0;
    DCH1ECON = 0;
    DCH1INT = 0;
    DCH1SSA = 0;
    DCH1DSA = 0;
    DCH1SSIZ = 0;
    DCH1DSIZ = 0;
    DCH1SPTR = 0;
    DCH1DPTR = 0;
    DCH1CSIZ = 0;
    DCH1CPTR = 0;
    DCH1DAT = 0;

    // Disable timer and restore default register states
    T8CON = 0;
    T9CON = 0;

    // Disable interrupts
    EVIC_SourceDisable(INT_SOURCE_DMA0);
    EVIC_SourceStatusClear(INT_SOURCE_DMA0);
}

/**
 * @brief Triggers the read callback to be called if any data is available.
 */
void Uart4DmaRXRead(void) {
    DCH0INTbits.CHTAIF = 1; // trigger transfer abort interrupt
}

/**
 * @brief DMA interrupt handler. This function should be called by the ISR
 * implementation generated by MPLAB Harmony.
 */
void Dma0InterruptHandler(void) {

    // Block transfer complete
    if (DCH0INTbits.CHBCIF == 1) {
        BlockTransferComplete();
        DCH0INTbits.CHBCIF = 0;
    }

    // Transfer aborted
    if (DCH0INTbits.CHTAIF == 1) {
        TransferAborted();
        DCH0INTbits.CHTAIF = 0;
    }

    // Re-enable channel
    EVIC_SourceStatusClear(INT_SOURCE_DMA0);
    DCH0CONbits.CHEN = 1;
}

/**
 * @brief Block transfer complete.
 */
static inline __attribute__((always_inline)) void BlockTransferComplete(void) {
    size_t numberOfBytes;
    if (DCH0ECONbits.PATEN == 1) {
        numberOfBytes = 0;
        while (true) {
            if (readBuffer[numberOfBytes] == DCH0DAT) {
                numberOfBytes++;
                break;
            }
            if (numberOfBytes >= DCH0DSIZ) {
                break;
            }
            numberOfBytes++;
        }
    } else {
        numberOfBytes = DCH0DSIZ;
    }
    if (read != NULL) {
        read(readBuffer, numberOfBytes);
    }
}

/**
 * @brief Transfer aborted.
 */
static inline __attribute__((always_inline)) void TransferAborted(void) {
    if (DCH0DPTR == 0) { // if no data received
        return;
    }
    const size_t numberOfBytes = DCH0DPTR;
    DCH0ECONbits.CABORT = 1; // reset DMA channel
    if (read != NULL) {
        read(readBuffer, numberOfBytes);
    }
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t Uart4DmaRXGetWriteAvailable(void) {
    return FifoGetWriteAvailable(&writeFifo);
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @return Result.
 */
 FifoResult Uart4DmaRXWrite(const void* const data, const size_t numberOfBytes) {
    const FifoResult result = FifoWrite(&writeFifo, data, numberOfBytes);
    EVIC_SourceEnable(INT_SOURCE_UART4_TX);
    return result;
}

/**
 * @brief Writes a byte to the write buffer.
 * @param byte Byte.
 * @return Result.
 */
FifoResult Uart4DmaRXWriteByte(const uint8_t byte) {
    const FifoResult result = FifoWriteByte(&writeFifo, byte);
    EVIC_SourceEnable(INT_SOURCE_UART4_TX);
    return result;
}

/**
 * @brief Clears the write buffer.
 */
void Uart4DmaRXClearWriteBuffer(void) {
    FifoClear(&writeFifo);
}

/**
 * @brief Returns true if the hardware receive buffer has overrun. Calling this
 * function will reset the flag.
 * @return True if the hardware receive buffer has overrun.
 */
bool Uart4DmaRXHasReceiveBufferOverrun(void) {
    if (U4STAbits.OERR == 1) {
        U4STAbits.OERR = 0;
        return true;
    }
    return false;
}

/**
 * @brief Returns true if all data has been transmitted.
 * @return True if all data has been transmitted.
 */
bool Uart4DmaRXTransmitionComplete(void) {
    return (EVIC_SourceIsEnabled(INT_SOURCE_UART4_TX) == false) && (U4STAbits.TRMT == 1);
}

#ifdef _UART_4_VECTOR

/**
 * @brief UART RX and TX interrupt handler. This function should be called by
 * the ISR implementation generated by MPLAB Harmony.
 */
void Uart4InterruptHandler(void) {
    TXInterruptTasks();
}

#else

/**
 * @brief UART TX interrupt handler. This function should be called by the
 * ISR implementation generated by MPLAB Harmony.
 */
void Uart4TXInterruptHandler(void) {
    TXInterruptTasks();
}

#endif

/**
 * @brief UART TX interrupt tasks.
 */
static inline __attribute__((always_inline)) void TXInterruptTasks(void) {
    EVIC_SourceDisable(INT_SOURCE_UART4_TX); // disable TX interrupt to avoid nested interrupt
    EVIC_SourceStatusClear(INT_SOURCE_UART4_TX);
    while (U4STAbits.UTXBF == 0) { // while transmit buffer not full
        if (FifoGetReadAvailable(&writeFifo) == 0) { // if write buffer empty
            return;
        }
        U4TXREG = FifoReadByte(&writeFifo);
    }
    EVIC_SourceEnable(INT_SOURCE_UART4_TX); // re-enable TX interrupt
}

//------------------------------------------------------------------------------
// End of file
