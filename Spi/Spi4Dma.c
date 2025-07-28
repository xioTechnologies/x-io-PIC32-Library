/**
 * @file Spi4Dma.c
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "definitions.h"
#include "Spi4Dma.h"
#include "sys/kmem.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Uncomment this line to enable printing of transfers.
 */
//#define PRINT_TRANSFERS

//------------------------------------------------------------------------------
// Variables

const Spi spi4Dma = {
    .transfer = Spi4DmaTransfer,
    .transferInProgress = Spi4DmaTransferInProgress,
};
static GPIO_PIN csPin;
#ifdef PRINT_TRANSFERS
static void* data;
static size_t numberOfBytes;
#endif
static void (*transferComplete)(void);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 */
void Spi4DmaInitialise(const SpiSettings * const settings) {

    // Ensure default register states
    Spi4DmaDeinitialise();

    // Configure SPI
    SPI4CONbits.MSTEN = 1; // host mode
    SPI4CONbits.ENHBUF = 1; // enhanced Buffer mode is enabled
    SPI4CONbits.SMP = 1; // input data sampled at end of data output time
    SPI4CONbits.CKP = settings->clockPolarity;
    SPI4CONbits.CKE = settings->clockPhase;
    SPI4CONbits.STXISEL = 0b11; // interrupt is generated when the buffer is not full (has one or more empty elements)
    SPI4CONbits.SRXISEL = 0b01; // interrupt is generated when the buffer is not empty
    SPI4BRG = SpiCalculateSpixbrg(settings->clockFrequency);
    SPI4CONbits.ON = 1;

    // Enable DMA
    DMACONbits.ON = 1;

    // Configure TX DMA channel
#ifdef _SPI4_TX_IRQ
    DCH0ECONbits.CHSIRQ = _SPI4_TX_IRQ;
#else
    DCH0ECONbits.CHSIRQ = _SPI4_TX_VECTOR;
#endif
    DCH0ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH0DSA = KVA_TO_PA(&SPI4BUF); // destination address
    DCH0DSIZ = 1; // destination size
    DCH0CSIZ = 1; // transfers per event

    // Configure RX DMA channel
#ifdef _SPI4_RX_IRQ
    DCH1ECONbits.CHSIRQ = _SPI4_RX_IRQ;
#else
    DCH1ECONbits.CHSIRQ = _SPI4_RX_VECTOR;
#endif
    DCH1ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH1SSA = KVA_TO_PA(&SPI4BUF); // source address
    DCH1SSIZ = 1; // source size
    DCH1CSIZ = 1; // transfers per event
    DCH1INTbits.CHBCIE = 1; // channel Block Transfer Complete Interrupt Enable bit

    // Configure RX DMA channel interrupt
    EVIC_SourceEnable(INT_SOURCE_DMA1);
}

/**
 * @brief Deinitialises the module.
 */
void Spi4DmaDeinitialise(void) {

    // Disable SPI and restore default register states
    SPI4CON = 0;
    SPI4CON2 = 0;
    SPI4STAT = 0;
    SPI4BRG = 0;

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

    // Disable interrupt
    EVIC_SourceDisable(INT_SOURCE_DMA1);
    EVIC_SourceStatusClear(INT_SOURCE_DMA1);
}

/**
 * @brief Transfers data. The data will be overwritten with the received data.
 * The data must be declared __attribute__((coherent)) for PIC32MZ devices.
 * This function must not be called while a transfer is in progress. The
 * transfer complete callback will be called from within an interrupt once the
 * transfer is complete.
 * @param csPin_ CS pin.
 * @param data_ Data.
 * @param numberOfBytes_ Number of bytes.
 * @param transferComplete_ Transfer complete callback.
 */
void Spi4DmaTransfer(const GPIO_PIN csPin_, void* const data_, const size_t numberOfBytes_, void (*transferComplete_)(void)) {

    // Store arguments
    csPin = csPin_;
#ifdef PRINT_TRANSFERS
    data = data_;
    numberOfBytes = numberOfBytes_;
#endif
    transferComplete = transferComplete_;

    // Print
#ifdef PRINT_TRANSFERS
    SpiPrintTransfer(csPin, data, numberOfBytes);
#endif

    // Configure TX DMA channel
    DCH0SSA = KVA_TO_PA(data_); // source address
    DCH0SSIZ = numberOfBytes_; // source size

    // Configure RX DMA channel
    DCH1DSA = KVA_TO_PA(data_); // destination address
    DCH1DSIZ = numberOfBytes_; // destination size

    // Begin transfer
    if (csPin != GPIO_PIN_NONE) {
#ifdef SPI4_CS_ACTIVE_HIGH
        GPIO_PinSet(csPin);
#else
        GPIO_PinClear(csPin);
#endif
    }
    DCH1INTbits.CHBCIF = 0; // clear RX DMA channel interrupt flag
    DCH1CONbits.CHEN = 1; // enable RX DMA channel
    DCH0CONbits.CHEN = 1; // enable TX DMA channel to begin transfer
}

/**
 * @brief DMA interrupt handler. This function should be called by the ISR
 * implementation generated by MPLAB Harmony.
 */
void Dma1InterruptHandler(void) {
    EVIC_SourceStatusClear(INT_SOURCE_DMA1); // clear interrupt flag first because transfer complete callback may start new transfer
    if (csPin != GPIO_PIN_NONE) {
#ifdef SPI4_CS_ACTIVE_HIGH
        GPIO_PinClear(csPin);
#else
        GPIO_PinSet(csPin);
#endif
    }
#ifdef PRINT_TRANSFERS
    SpiPrintTransferComplete(data, numberOfBytes);
#endif
    if (transferComplete != NULL) {
        transferComplete();
    }
}

/**
 * @brief Returns true while the transfer is in progress.
 * @return True while the transfer is in progress.
 */
bool Spi4DmaTransferInProgress(void) {
    return DCH1CONbits.CHEN == 1; // if RX DMA channel interrupt enabled
}

//------------------------------------------------------------------------------
// End of file
