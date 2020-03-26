/**
 * @file Uart3.c
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>
#include "system_definitions.h"
#include "Timer/Timer.h"
#include "Uart3Dma.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Function declarations

static inline __attribute__((always_inline)) void BlockTransferComplete();
static inline __attribute__((always_inline)) void TransferAborted();

//------------------------------------------------------------------------------
// Variables

static void (*readCallback)(const void* const data, const size_t numberOfBytes);
static uint8_t __attribute__((coherent)) readBuffer[1024]; // must be declared __attribute__((coherent)) for PIC32MZ devices

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 * @param readConditions Read conditions.
 */
void Uart3DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions) {

    // Ensure default register states
    Uart3DmaDisable();

    // Configure UART
    if (settings->rtsCtsEnabled == true) {
        U3MODEbits.UEN = 0b10; // UxTX, UxRX, UxCTS and UxRTS pins are enabled and used
    }
    if (settings->invertDataLines == true) {
        U3MODEbits.RXINV = 1; // UxRX Idle state is '0'
        U3STAbits.UTXINV = 1; // UxTX Idle state is '0'
    }
    U3MODEbits.PDSEL = settings->parityAndData;
    U3MODEbits.STSEL = settings->stopBits;
    U3MODEbits.BRGH = 1; // High-Speed mode - 4x baud clock enabled
    U3STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U3STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U3BRG = UartCalculateUxbrg(settings->baudRate);
    U3MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

    // Configure TX DMA channel
    DCH2ECONbits.CHSIRQ = INT_VECTOR_UART3_TX;
    DCH2ECONbits.SIRQEN = 1; // Start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH2DSA = KVA_TO_PA(&U3TXREG); // destination address
    DCH2DSIZ = 1; // destination size
    DCH2CSIZ = 1; // transfers per event

    // Limit read condition values to valid range
    UartDmaReadConditions validReadConditions = *readConditions;
    if (validReadConditions.numberOfBytes > sizeof (readBuffer)) {
        validReadConditions.numberOfBytes = sizeof (readBuffer);
    }
    if (validReadConditions.terminatingByte != -1) {
        validReadConditions.terminatingByte &= 0xFF;
    }
    if (validReadConditions.timeout > 1000) {
        validReadConditions.timeout = 1000;
    }

    // Configure RX DMA channel
    DCH3ECONbits.CHAIRQ = INT_VECTOR_T9;
    DCH3ECONbits.CHSIRQ = INT_VECTOR_UART3_RX;
    DCH3ECONbits.SIRQEN = 1; // Start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH3ECONbits.AIRQEN = 1; // Channel transfer is aborted if an interrupt matching CHAIRQ occurs
    if (readConditions->terminatingByte != -1) {
        DCH3ECONbits.PATEN = 1; // Abort transfer and clear CHEN on pattern match
    }
    DCH3SSA = KVA_TO_PA(&U3RXREG); // source address
    DCH3DSA = KVA_TO_PA(readBuffer); // destination address
    DCH3SSIZ = 1; // source size
    DCH3DSIZ = validReadConditions.numberOfBytes; // destination size
    DCH3CSIZ = 1; // transfers per event
    DCH3DAT = validReadConditions.terminatingByte; // pattern data
    DCH3INTbits.CHBCIE = 1; // Channel Block Transfer Complete Interrupt Enable bit
    DCH3INTbits.CHTAIE = 1; // Channel Transfer Abort Interrupt Enable bit
    DCH3CONbits.CHEN = 1; // Channel is enabled

    // Calculate timer reset value
    static uint32_t __attribute__((coherent)) timerResetValue[1]; // must be declared __attribute__((coherent)) for PIC32MZ devices
    timerResetValue[0] = UartDmaCalculateTimerResetValue(validReadConditions.timeout);

    // Configure timer DMA channel
    DCH4CONbits.CHAEN = 1; // Channel is continuously enabled, and not automatically disabled after a block transfer is complete
    DCH4ECONbits.CHSIRQ = INT_VECTOR_UART3_RX;
    DCH4ECONbits.SIRQEN = 1; // Start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH4SSA = KVA_TO_PA(timerResetValue); // source address
    DCH4DSA = KVA_TO_PA(&TMR8); // destination address
    DCH4SSIZ = sizeof (timerResetValue); // source size
    DCH4DSIZ = sizeof (timerResetValue); // destination size
    DCH4CSIZ = sizeof (timerResetValue); // transfers per event
    DCH4CONbits.CHEN = 1; // Channel is enabled

    // Configure timer
    T8CONbits.T32 = 1;
    T8CONbits.ON = 1;

    // Configure RX DMA channel interrupt
    SYS_INT_VectorPrioritySet(_DMA3_VECTOR, INT_PRIORITY_LEVEL4);
    SYS_INT_SourceStatusClear(INT_SOURCE_DMA_3);
    SYS_INT_SourceEnable(INT_SOURCE_DMA_3);
}

/**
 * @brief Disables the module.
 */
void Uart3DmaDisable() {

    // Disable UART and restore default register states
    U3MODE = 0;
    U3STA = 0;

    // Disable TX DMA channel and restore default register states
    DCH2CON = 0;
    DCH2ECON = 0;
    DCH2INT = 0;
    DCH2SSA = 0;
    DCH2DSA = 0;
    DCH2SSIZ = 0;
    DCH2DSIZ = 0;
    DCH2SPTR = 0;
    DCH2DPTR = 0;
    DCH2CSIZ = 0;
    DCH2CPTR = 0;
    DCH2DAT = 0;

    // Disable RX DMA channel and restore default register states
    DCH3CON = 0;
    DCH3ECON = 0;
    DCH3INT = 0;
    DCH3SSA = 0;
    DCH3DSA = 0;
    DCH3SSIZ = 0;
    DCH3DSIZ = 0;
    DCH3SPTR = 0;
    DCH3DPTR = 0;
    DCH3CSIZ = 0;
    DCH3CPTR = 0;
    DCH3DAT = 0;

    // Disable timer DMA channel and restore default register states
    DCH4CON = 0;
    DCH4ECON = 0;
    DCH4INT = 0;
    DCH4SSA = 0;
    DCH4DSA = 0;
    DCH4SSIZ = 0;
    DCH4DSIZ = 0;
    DCH4SPTR = 0;
    DCH4DPTR = 0;
    DCH4CSIZ = 0;
    DCH4CPTR = 0;
    DCH4DAT = 0;

    // Disable timer and restore default register states
    T8CON = 0;
    T9CON = 0;

    // Disable interrupt
    SYS_INT_SourceDisable(INT_SOURCE_DMA_3);
    SYS_INT_SourceStatusClear(INT_SOURCE_DMA_3);
}

/**
 * @brief Sets the read callback function.  This function is called from within
 * an interrupt each time a read condition is met.
 * @param read Read callback function.
 */
void Uart3DmaSetReadCallback(void (*read)(const void* const data, const size_t numberOfBytes)) {
    SYS_INT_SourceDisable(INT_SOURCE_DMA_3);
    readCallback = read;
    SYS_INT_SourceEnable(INT_SOURCE_DMA_3);
}

/**
 * @brief Triggers the read callback function to be called if any data is
 * available.
 */
void Uart3DmaRead() {
    DCH3INTbits.CHTAIF = 1; // trigger transfer abort interrupt
}

/**
 * @brief RX DMA channel interrupt.
 */
void __ISR(_DMA3_VECTOR) Dma3Interrupt() {

    // Block transfer complete
    if (DCH3INTbits.CHBCIF == 1) {
        BlockTransferComplete();
        DCH3INTbits.CHBCIF = 0;
    }

    // Transfer aborted
    if (DCH3INTbits.CHTAIF == 1) {
        TransferAborted();
        DCH3INTbits.CHTAIF = 0;
    }

    // Clear interrupt flag
    SYS_INT_SourceStatusClear(INT_SOURCE_DMA_3);

    // Re-enable channel
    DCH3CONbits.CHEN = 1;
}

/**
 * @brief Block transfer complete.
 */
static inline __attribute__((always_inline)) void BlockTransferComplete() {

    // Get number of bytes
    size_t numberOfBytes;
    if (DCH3ECONbits.PATEN == 1) {
        numberOfBytes = 0;
        while (true) {
            if (readBuffer[numberOfBytes] == DCH3DAT) {
                numberOfBytes++;
                break;
            }
            if (numberOfBytes >= DCH3DSIZ) {
                break;
            }
            numberOfBytes++;
        }
    } else {
        numberOfBytes = DCH3DSIZ;
    }

    // Callback function
    if (readCallback != NULL) {
        readCallback(readBuffer, numberOfBytes);
    }
}

/**
 * @brief Transfer aborted.
 */
static inline __attribute__((always_inline)) void TransferAborted() {

    // Do nothing if no data received
    if (DCH3DPTR == 0) {
        return;
    }

    // Get number of bytes
    const size_t numberOfBytes = DCH3DPTR;

    // Reset DMA channel
    DCH3ECONbits.CABORT = 1;

    // Callback function
    if (readCallback != NULL) {
        readCallback(readBuffer, numberOfBytes);
    }
}

/**
 * @brief Writes data.  The data must be declared __attribute__((coherent)) for
 * PIC32MZ devices.  This function must not be called while a write is in
 * progress.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void Uart3DmaWrite(const void* const data, const size_t numberOfBytes) {
    DCH2SSA = KVA_TO_PA(data); // source address
    DCH2SSIZ = numberOfBytes; // source size
    DCH2CONbits.CHEN = 1; // enable TX DMA channel
}

/**
 * @brief Returns true while data is being transferred to the hardware transmit
 * buffer.
 * @return True while data is being transferred to the hardware transmit buffer.
 */
bool Uart3DmaIsWriteInProgress() {
    return DCH2CONbits.CHEN == 1;
}

/**
 * @brief Returns true if the hardware receive buffer has overrun.  Calling this
 * function will reset the flag.
 * @return True if the hardware receive buffer has overrun.
 */
bool Uart3DmaHasReceiveBufferOverrun() {
    if (U3STAbits.OERR == 1) {
        U3STAbits.OERR = 0;
        return true;
    }
    return false;
}

/**
 * @brief Returns true if all data has been transmitted.
 * @return True if all data has been transmitted.
 */
bool Uart3DmaIsTransmitionComplete() {
    return (Uart3DmaIsWriteInProgress() == false) && (U3STAbits.TRMT == 1);
}

//------------------------------------------------------------------------------
// End of file
