/**
 * @file Uart3DmaTx.c
 * @author Seb Madgwick
 * @brief UART driver using DMA for PIC32 devices. DMA used for TX only.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "definitions.h"
#include "Fifo.h"
#include "sys/kmem.h"
#include "Uart3DmaTx.h"

//------------------------------------------------------------------------------
// Function declarations

static void WriteTransferComplete(void);
static inline __attribute__((always_inline)) void RxInterruptTasks(void);

//------------------------------------------------------------------------------
// Variables

static bool receiveBufferOverrun;
static uint8_t readData[UART3_READ_BUFFER_SIZE];
static Fifo readFifo = {.data = readData, .dataSize = sizeof (readData)};
static uint8_t writeData[UART3_WRITE_BUFFER_SIZE];
static Fifo writeFifo = {.data = writeData, .dataSize = sizeof (writeData)};
static void (*writeTransferComplete)(void);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 */
void Uart3DmaTxInitialise(const UartSettings * const settings) {

    // Ensure default register states
    Uart3DmaTxDeinitialise();

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
    U3STAbits.URXISEL = 0b01; // interrupt flag bit is asserted while receive buffer is 1/2 or more full (i.e., has 4 or more data characters)
    U3STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U3STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U3BRG = UartCalculateUxbrg(settings->baudRate);
    U3MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

    // Enable DMA
    DMACONbits.ON = 1;

    // Configure TX DMA channel
    DCH0ECONbits.CHSIRQ = _UART3_TX_VECTOR; // channel transfer start IRQ
    DCH0ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH0DSA = KVA_TO_PA(&U3TXREG); // destination address
    DCH0DSIZ = 1; // destination size
    DCH0CSIZ = 1; // transfers per event
    DCH0INTbits.CHBCIE = 1; // channel block transfer complete interrupt enable bit

    // Enable interrupts
    EVIC_SourceEnable(INT_SOURCE_UART3_RX);
    EVIC_SourceEnable(INT_SOURCE_DMA0);
}

/**
 * @brief Deinitialises the module.
 */
void Uart3DmaTxDeinitialise(void) {

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

    // Disable interrupts
    EVIC_SourceDisable(INT_SOURCE_UART3_RX);
    EVIC_SourceDisable(INT_SOURCE_DMA0);
    EVIC_SourceStatusClear(INT_SOURCE_UART3_RX);
    EVIC_SourceStatusClear(INT_SOURCE_DMA0);

    // Clear buffers
    receiveBufferOverrun = false;
    Uart3DmaTxClearReadBuffer();
    Uart3DmaTxClearWriteBuffer();
}

/**
 * @brief Returns the number of bytes available in the read buffer.
 * @return Number of bytes available in the read buffer.
 */
size_t Uart3DmaTxAvailableRead(void) {

    // Trigger RX interrupt if receive buffer not empty
    if (U3STAbits.URXDA == 1) {
        EVIC_SourceEnable(INT_SOURCE_UART3_RX);
        EVIC_SourceStatusSet(INT_SOURCE_UART3_RX);
    }

    // Clear receive buffer overrun flag
    if (U3STAbits.OERR == 1) {
        U3STAbits.OERR = 0;
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
size_t Uart3DmaTxRead(void* const destination, size_t numberOfBytes) {
    Uart3DmaTxAvailableRead(); // process receive buffer
    return FifoRead(&readFifo, destination, numberOfBytes);
}

/**
 * @brief Reads a byte from the read buffer. This function must only be called
 * if there are bytes available in the read buffer.
 * @return Byte.
 */
uint8_t Uart3DmaTxReadByte(void) {
    return FifoReadByte(&readFifo);
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t Uart3DmaTxAvailableWrite(void) {
    return FifoAvailableWrite(&writeFifo);
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @return Result.
 */
FifoResult Uart3DmaTxWrite(const void* const data, const size_t numberOfBytes) {
    const FifoResult result = FifoWrite(&writeFifo, data, numberOfBytes);
    if (Uart3DmaTxWriteTransferInProgress() == false) {
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
        Uart3DmaTxWriteTransfer(data, numberOfBytes, WriteTransferComplete);
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
void Uart3DmaTxWriteTransfer(const void* const data, const size_t numberOfBytes, void (*const writeTransferComplete_) (void)) {
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
bool Uart3DmaTxWriteTransferInProgress(void) {
    return DCH0CONbits.CHEN == 1;
}

/**
 * @brief Clears the read buffer and resets the read buffer overrun flag.
 */
void Uart3DmaTxClearReadBuffer(void) {
    FifoClear(&readFifo);
    Uart3DmaTxReceiveBufferOverrun(); // clear flag
}

/**
 * @brief Clears the write buffer.
 */
void Uart3DmaTxClearWriteBuffer(void) {
    FifoClear(&writeFifo);
}

/**
 * @brief Returns true if the receive buffer has overrun. Calling this function
 * will reset the flag.
 * @return True if the receive buffer has overrun.
 */
bool Uart3DmaTxReceiveBufferOverrun(void) {
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
bool Uart3DmaTxTransmissionComplete(void) {
    return (Uart3DmaTxWriteTransferInProgress() == false) && (U3STAbits.TRMT == 1);
}

#ifdef _UART_3_VECTOR

/**
 * @brief UART RX and TX interrupt handler. This function should be called by
 * the ISR implementation generated by MPLAB Harmony.
 */
void Uart3InterruptHandler(void) {
    if (EVIC_SourceStatusGet(INT_SOURCE_UART3_RX)) {
        RxInterruptTasks();
    }
}

#else

/**
 * @brief UART RX interrupt handler. This function should be called by the ISR
 * implementation generated by MPLAB Harmony.
 */
void Uart3RxInterruptHandler(void) {
    RxInterruptTasks();
}

#endif

/**
 * @brief UART RX interrupt tasks.
 */
static inline __attribute__((always_inline)) void RxInterruptTasks(void) {
    while (U3STAbits.URXDA == 1) { // while data available in receive buffer
        if (FifoAvailableWrite(&readFifo) == 0) { // if read buffer full
            EVIC_SourceDisable(INT_SOURCE_UART3_RX);
            break;
        }
        FifoWriteByte(&readFifo, U3RXREG);
    }
    EVIC_SourceStatusClear(INT_SOURCE_UART3_RX);
}

//------------------------------------------------------------------------------
// End of file
