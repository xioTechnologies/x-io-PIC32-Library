/**
 * @file Uart3Dma.c
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "definitions.h"
#include <stdint.h>
#include "sys/kmem.h"
#include "Timer/Timer.h"
#include "Uart3Dma.h"

//------------------------------------------------------------------------------
// Function declarations

static void WriteTransferComplete(void);
static inline __attribute__((always_inline)) void BlockTransferComplete(void);
static inline __attribute__((always_inline)) void TransferAborted(void);

//------------------------------------------------------------------------------
// Variables

#ifdef UART3_DMA_TIMEOUT_POLL
static uint64_t readTimeoutTicks;
static uint64_t readTimeoutExpiry;
#endif
static bool receiveBufferOverrun;
static void (*read)(const void* const data, const size_t numberOfBytes);
static uint8_t __attribute__((coherent)) readData[UART3_DMA_READ_TRANSFER_SIZE]; // must be declared __attribute__((coherent)) for PIC32MZ devices
static uint8_t writeData[UART3_WRITE_BUFFER_SIZE];
static Fifo writeFifo = {.data = writeData, .dataSize = sizeof (writeData)};
static void (*writeTransferComplete)(void);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 * @param readConditions Read conditions.
 * @param read Read callback. NULL if unused.
 */
void Uart3DmaInitialise(const UartSettings * const settings, const UartDmaReadConditions * const readConditions, void (*const read_) (const void* const data, const size_t numberOfBytes)) {

    // Ensure default register states
    Uart3DmaDeinitialise();

    // Configure UART
    if (settings->rtsCtsEnabled) {
        U3MODEbits.UEN = 0b10; // UxTX, UxRX, UxCTS and UxRTS pins are enabled and used
    }
    if (settings->invertTxRx) {
        U3MODEbits.RXINV = 1; // UxRX Idle state is '0'
        U3STAbits.UTXINV = 1; // UxTX Idle state is '0'
    }
    U3MODEbits.PDSEL = settings->parityAndData;
    U3MODEbits.STSEL = settings->stopBits;
    U3MODEbits.BRGH = 1; // high-Speed mode - 4x baud clock enabled
    U3STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U3STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U3BRG = UartCalculateUxbrg(settings->baudRate);
    U3MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

    // Store read arguments
#ifdef UART3_DMA_TIMEOUT_POLL
    readTimeoutTicks = (uint64_t) readConditions->timeout * TIMER_TICKS_PER_MILLISECOND;
#endif
    read = read_;

    // Enable DMA
    DMACONbits.ON = 1;

    // Configure TX DMA channel
    DCH0ECONbits.CHSIRQ = _UART3_TX_VECTOR; // channel transfer start IRQ
    DCH0ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH0DSA = KVA_TO_PA(&U3TXREG); // destination address
    DCH0DSIZ = 1; // destination size
    DCH0CSIZ = 1; // transfers per event
    DCH0INTbits.CHBCIE = 1; // channel block transfer complete interrupt enable bit

    // Configure RX DMA channel
#ifndef UART3_DMA_TIMEOUT_POLL
    DCH1ECONbits.CHAIRQ = _TIMER_9_VECTOR; // channel transfer abort IRQ
    DCH1ECONbits.AIRQEN = 1; // channel transfer is aborted if an interrupt matching CHAIRQ occurs
#endif
    DCH1ECONbits.CHSIRQ = _UART3_RX_VECTOR; // channel transfer start IRQ
    DCH1ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    if (readConditions->termination != -1) {
        DCH1ECONbits.PATEN = 1; // abort transfer and clear CHEN on pattern match
    }
    DCH1SSA = KVA_TO_PA(&U3RXREG); // source address
    DCH1DSA = KVA_TO_PA(readData); // destination address
    DCH1SSIZ = 1; // source size
    DCH1DSIZ = readConditions->numberOfBytes > sizeof (readData) ? sizeof (readData) : readConditions->numberOfBytes; // destination size
    DCH1CSIZ = 1; // transfers per event
    DCH1DAT = readConditions->termination & 0xFF; // pattern data
    DCH1INTbits.CHBCIE = 1; // channel block transfer complete interrupt enable bit
    DCH1INTbits.CHTAIE = 1; // channel transfer abort interrupt enable bit
    DCH1CONbits.CHEN = 1; // channel is enabled

    // Calculate timer reset value
#ifndef UART3_DMA_TIMEOUT_POLL
    static uint32_t __attribute__((coherent)) timerReset[1]; // must be declared __attribute__((coherent)) for PIC32MZ devices
    timerReset[0] = UartDmaCalculateTimerReset(readConditions->timeout);

    // Configure timer DMA channel
    DCH2CONbits.CHAEN = 1; // channel is continuously enabled, and not automatically disabled after a block transfer is complete
    DCH2ECONbits.CHSIRQ = _UART3_RX_VECTOR; // channel transfer start IRQ
    DCH2ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH2SSA = KVA_TO_PA(timerReset); // source address
    DCH2DSA = KVA_TO_PA(&TMR8); // destination address
    DCH2SSIZ = sizeof (timerReset); // source size
    DCH2DSIZ = sizeof (timerReset); // destination size
    DCH2CSIZ = sizeof (timerReset); // transfers per event
    DCH2CONbits.CHEN = 1; // channel is enabled

    // Configure timer
    T8CONbits.T32 = 1;
    T8CONbits.ON = 1;
#endif

    // Enable interrupts
    EVIC_SourceEnable(INT_SOURCE_DMA0);
    EVIC_SourceEnable(INT_SOURCE_DMA1);
}

/**
 * @brief Deinitialises the module.
 */
void Uart3DmaDeinitialise(void) {

    // Disable UART and restore default register states
    U3MODE = 0;
    U3STA = 0;

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
#ifndef UART3_DMA_TIMEOUT_POLL
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
#endif

    // Disable interrupts
    EVIC_SourceDisable(INT_SOURCE_DMA0);
    EVIC_SourceDisable(INT_SOURCE_DMA1);
    EVIC_SourceStatusClear(INT_SOURCE_DMA0);
    EVIC_SourceStatusClear(INT_SOURCE_DMA1);

    // Clear buffer
    receiveBufferOverrun = false;
    Uart3DmaClearWriteBuffer();
}

/**
 * @brief Module tasks. This function should be called repeatedly within the
 * main program loop.
 */
void Uart3DmaTasks(void) {

    // Clear receive buffer overrun flag
    if (U3STAbits.OERR == 1) {
        U3STAbits.OERR = 0;
        receiveBufferOverrun = true;
    }

    // Do nothing else if no data available
#ifdef UART3_DMA_TIMEOUT_POLL
    const size_t available = DCH1DPTR;
    static size_t previous;
    if (available == 0) {
        previous = 0;
        return;
    }

    // Reset timeout if new data received
    if (available != previous) {
        readTimeoutExpiry = TimerGetTicks64() + readTimeoutTicks;
        previous = available;
        return;
    }

    // Wait for timeout
    if (TimerGetTicks64() < readTimeoutExpiry) {
        return;
    }

    // Read
    DCH1INTbits.CHTAIF = 1; // trigger transfer abort interrupt
    previous = 0;
#endif
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
        while (numberOfBytes < DCH1DSIZ) { // count number of bytes up to and including termination
            if (readData[numberOfBytes++] == DCH1DAT) {
                break;
            }
        }
    } else {
        numberOfBytes = DCH1DSIZ;
    }
    if (read != NULL) {
        read(readData, numberOfBytes);
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
        read(readData, numberOfBytes);
    }
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t Uart3DmaAvailableWrite(void) {
    return FifoAvailableWrite(&writeFifo);
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @return Result.
 */
FifoResult Uart3DmaWrite(const void* const data, const size_t numberOfBytes) {
    const FifoResult result = FifoWrite(&writeFifo, data, numberOfBytes);
    if (Uart3DmaWriteTransferInProgress() == false) {
        WriteTransferComplete();
    }
    return result;
}

/**
 * @brief Write transfer complete callback.
 */
static void WriteTransferComplete(void) {
    static __attribute__((coherent)) uint8_t data[UART3_DMA_WRITE_TRANSFER_SIZE];
    const size_t numberOfBytes = FifoRead(&writeFifo, data, sizeof (data));
    if (numberOfBytes > 0) {
        Uart3DmaWriteTransfer(data, numberOfBytes, WriteTransferComplete);
    }
}

/**
 * @brief Writes data directly to the transmit buffer through a DMA transfer.
 * The data must be declared __attribute__((coherent)) for PIC32MZ devices. This
 * function must not be called while a transfer is in progress.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @param writeTransferComplete_ Write transfer complete callback. NULL if
 * unused.
 */
void Uart3DmaWriteTransfer(const void* const data, const size_t numberOfBytes, void (*const writeTransferComplete_) (void)) {
    writeTransferComplete = writeTransferComplete_;
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
    if (writeTransferComplete != NULL) {
        writeTransferComplete();
    }
}

/**
 * @brief Returns true while data is being transferred to the transmit buffer.
 * @return True while data is being transferred to the transmit buffer.
 */
bool Uart3DmaWriteTransferInProgress(void) {
    return DCH0CONbits.CHEN == 1;
}

/**
 * @brief Clears the write buffer.
 */
void Uart3DmaClearWriteBuffer(void) {
    FifoClear(&writeFifo);
}

/**
 * @brief Returns true if the receive buffer has overrun. Calling this function
 * will reset the flag.
 * @return True if the receive buffer has overrun.
 */
bool Uart3DmaReceiveBufferOverrun(void) {
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
bool Uart3DmaTransmissionComplete(void) {
    return (Uart3DmaWriteTransferInProgress() == false) && (U3STAbits.TRMT == 1);
}

//------------------------------------------------------------------------------
// End of file
