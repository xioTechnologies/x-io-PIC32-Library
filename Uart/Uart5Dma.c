/**
 * @file Uart5Dma.c
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include <stdint.h>
#include "sys/kmem.h"
#include "Timer/Timer.h"
#include "Uart5Dma.h"

//------------------------------------------------------------------------------
// Function declarations

static inline __attribute__((always_inline)) void BlockTransferComplete(void);
static inline __attribute__((always_inline)) void TransferAborted(void);

//------------------------------------------------------------------------------
// Variables

static void (*read)(const void* const data, const size_t numberOfBytes);
static uint8_t __attribute__((coherent)) readBuffer[1024]; // must be declared __attribute__((coherent)) for PIC32MZ devices
static void (*writeComplete)(void);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 * @param readConditions Read conditions.
 * @param read Read callback.
 */
void Uart5DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*const read_) (const void* const data, const size_t numberOfBytes)) {

    // Ensure default register states
    Uart5DmaDeinitialise();

    // Configure UART
    if (settings->rtsCtsEnabled) {
        U5MODEbits.UEN = 0b10; // UxTX, UxRX, UxCTS and UxRTS pins are enabled and used
    }
    if (settings->invertTXRX) {
        U5MODEbits.RXINV = 1; // UxRX Idle state is '0'
        U5STAbits.UTXINV = 1; // UxTX Idle state is '0'
    }
    U5MODEbits.PDSEL = settings->parityAndData;
    U5MODEbits.STSEL = settings->stopBits;
    U5MODEbits.BRGH = 1; // high-Speed mode - 4x baud clock enabled
    U5STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U5STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U5BRG = UartCalculateUxbrg(settings->baudRate);
    U5MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

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

    // Configure TX DMA channel
    DCH0ECONbits.CHSIRQ = _UART5_TX_VECTOR;
    DCH0ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH0DSA = KVA_TO_PA(&U5TXREG); // destination address
    DCH0DSIZ = 1; // destination size
    DCH0CSIZ = 1; // transfers per event
    DCH0INTbits.CHBCIE = 1; // channel Block Transfer Complete Interrupt Enable bit

    // Configure RX DMA channel
    DCH1ECONbits.CHAIRQ = _TIMER_9_VECTOR;
    DCH1ECONbits.CHSIRQ = _UART5_RX_VECTOR;
    DCH1ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH1ECONbits.AIRQEN = 1; // channel transfer is aborted if an interrupt matching CHAIRQ occurs
    if (readConditions->terminationByte != -1) {
        DCH1ECONbits.PATEN = 1; // abort transfer and clear CHEN on pattern match
    }
    DCH1SSA = KVA_TO_PA(&U5RXREG); // source address
    DCH1DSA = KVA_TO_PA(readBuffer); // destination address
    DCH1SSIZ = 1; // source size
    DCH1DSIZ = validReadConditions.numberOfBytes; // destination size
    DCH1CSIZ = 1; // transfers per event
    DCH1DAT = validReadConditions.terminationByte; // pattern data
    DCH1INTbits.CHBCIE = 1; // channel Block Transfer Complete Interrupt Enable bit
    DCH1INTbits.CHTAIE = 1; // channel Transfer Abort Interrupt Enable bit
    DCH1CONbits.CHEN = 1; // channel is enabled

    // Calculate timer reset value
    static uint32_t __attribute__((coherent)) timerResetValue[1]; // must be declared __attribute__((coherent)) for PIC32MZ devices
    timerResetValue[0] = UartDmaCalculateTimerResetValue(validReadConditions.timeout);

    // Configure timer DMA channel
    DCH2CONbits.CHAEN = 1; // channel is continuously enabled, and not automatically disabled after a block transfer is complete
    DCH2ECONbits.CHSIRQ = _UART5_RX_VECTOR;
    DCH2ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH2SSA = KVA_TO_PA(timerResetValue); // source address
    DCH2DSA = KVA_TO_PA(&TMR8); // destination address
    DCH2SSIZ = sizeof (timerResetValue); // source size
    DCH2DSIZ = sizeof (timerResetValue); // destination size
    DCH2CSIZ = sizeof (timerResetValue); // transfers per event
    DCH2CONbits.CHEN = 1; // channel is enabled

    // Configure timer
    T8CONbits.T32 = 1;
    T8CONbits.ON = 1;

    // Enable interrupts
    EVIC_SourceEnable(INT_SOURCE_DMA1);
    EVIC_SourceEnable(INT_SOURCE_DMA0);
}

/**
 * @brief Deinitialises the module.
 */
void Uart5DmaDeinitialise(void) {

    // Disable UART and restore default register states
    U5MODE = 0;
    U5STA = 0;

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

    // Disable RX DMA channel and restore default register states
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

    // Disable timer DMA channel and restore default register states
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

    // Disable timer and restore default register states
    T8CON = 0;
    T9CON = 0;

    // Disable interrupts
    EVIC_SourceDisable(INT_SOURCE_DMA1);
    EVIC_SourceStatusClear(INT_SOURCE_DMA1);
}

/**
 * @brief Triggers the read callback to be called if any data is available.
 */
void Uart5DmaRead(void) {
    DCH1INTbits.CHTAIF = 1; // trigger transfer abort interrupt
}

/**
 * @brief DMA interrupt handler. This function should be called by the ISR
 * implementation generated by MPLAB Harmony.
 */
void Dma1InterruptHandler(void) {

    // Block transfer complete
    if (DCH1INTbits.CHBCIF == 1) {
        BlockTransferComplete();
        DCH1INTbits.CHBCIF = 0;
    }

    // Transfer aborted
    if (DCH1INTbits.CHTAIF == 1) {
        TransferAborted();
        DCH1INTbits.CHTAIF = 0;
    }

    // Re-enable channel
    EVIC_SourceStatusClear(INT_SOURCE_DMA1);
    DCH1CONbits.CHEN = 1;
}

/**
 * @brief Block transfer complete.
 */
static inline __attribute__((always_inline)) void BlockTransferComplete(void) {
    size_t numberOfBytes;
    if (DCH1ECONbits.PATEN == 1) {
        numberOfBytes = 0;
        while (true) {
            if (readBuffer[numberOfBytes] == DCH1DAT) {
                numberOfBytes++;
                break;
            }
            if (numberOfBytes >= DCH1DSIZ) {
                break;
            }
            numberOfBytes++;
        }
    } else {
        numberOfBytes = DCH1DSIZ;
    }
    if (read != NULL) {
        read(readBuffer, numberOfBytes);
    }
}

/**
 * @brief Transfer aborted.
 */
static inline __attribute__((always_inline)) void TransferAborted(void) {
    if (DCH1DPTR == 0) { // if no data received
        return;
    }
    const size_t numberOfBytes = DCH1DPTR;
    DCH1ECONbits.CABORT = 1; // reset DMA channel
    if (read != NULL) {
        read(readBuffer, numberOfBytes);
    }
}

/**
 * @brief Writes data. The data must be declared __attribute__((coherent)) for
 * PIC32MZ devices. This function must not be called while a write is in
 * progress.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @param writeComplete_ Write complete callback.
 */
void Uart5DmaWrite(const void* const data, const size_t numberOfBytes, void (*const writeComplete_) (void)) {
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
bool Uart5DmaWriteInProgress(void) {
    return DCH0CONbits.CHEN == 1;
}

/**
 * @brief Returns true if the hardware receive buffer has overrun. Calling this
 * function will reset the flag.
 * @return True if the hardware receive buffer has overrun.
 */
bool Uart5DmaReceiveBufferOverrun(void) {
    if (U5STAbits.OERR == 1) {
        U5STAbits.OERR = 0;
        return true;
    }
    return false;
}

/**
 * @brief Returns true if all data has been transmitted.
 * @return True if all data has been transmitted.
 */
bool Uart5DmaTransmitionComplete(void) {
    return (Uart5DmaWriteInProgress() == false) && (U5STAbits.TRMT == 1);
}

//------------------------------------------------------------------------------
// End of file
