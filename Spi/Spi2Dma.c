/**
 * @file Spi2Dma.c
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "definitions.h"
#include "Spi2Dma.h"
#include "sys/kmem.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Uncomment this line to print transfers.
 */
//#define PRINT_TRANSFERS

//------------------------------------------------------------------------------
// Variables

const Spi spi2Dma = {
    .transfer = Spi2DmaTransfer,
    .transferInProgress = Spi2DmaTransferInProgress,
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
void Spi2DmaInitialise(const SpiSettings * const settings) {

    // Ensure default register states
    Spi2DmaDeinitialise();

    // Configure SPI
    SPI2CONbits.MSTEN = 1; // host mode
    SPI2CONbits.ENHBUF = 1; // enhanced Buffer mode is enabled
    SPI2CONbits.SMP = 1; // input data sampled at end of data output time
    SPI2CONbits.CKP = settings->clockPolarity;
    SPI2CONbits.CKE = settings->clockPhase;
    SPI2CONbits.STXISEL = 0b11; // interrupt is generated when the buffer is not full (has one or more empty elements)
    SPI2CONbits.SRXISEL = 0b01; // interrupt is generated when the buffer is not empty
    SPI2BRG = SpiCalculateSpixbrg(settings->clockFrequency);
    SPI2CONbits.ON = 1;

    // Enable DMA
    DMACONbits.ON = 1;

    // Configure TX DMA channel
#ifdef _SPI2_TX_IRQ
    DCH0ECONbits.CHSIRQ = _SPI2_TX_IRQ;
#else
    DCH0ECONbits.CHSIRQ = _SPI2_TX_VECTOR;
#endif
    DCH0ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH0DSA = KVA_TO_PA(&SPI2BUF); // destination address
    DCH0DSIZ = 1; // destination size
    DCH0CSIZ = 1; // transfers per event

    // Configure RX DMA channel
#ifdef _SPI2_RX_IRQ
    DCH1ECONbits.CHSIRQ = _SPI2_RX_IRQ;
#else
    DCH1ECONbits.CHSIRQ = _SPI2_RX_VECTOR;
#endif
    DCH1ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH1SSA = KVA_TO_PA(&SPI2BUF); // source address
    DCH1SSIZ = 1; // source size
    DCH1CSIZ = 1; // transfers per event
    DCH1INTbits.CHBCIE = 1; // channel Block Transfer Complete Interrupt Enable bit

    // Configure RX DMA channel interrupt
    EVIC_SourceEnable(INT_SOURCE_DMA1);
}

/**
 * @brief Deinitialises the module.
 */
void Spi2DmaDeinitialise(void) {

    // Disable SPI and restore default register states
    SPI2CON = 0;
    SPI2CON2 = 0;
    SPI2STAT = 0;
    SPI2BRG = 0;

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
 * @param transferComplete_ Transfer complete callback. NULL if unused.
 */
void Spi2DmaTransfer(const GPIO_PIN csPin_, volatile void* const data_, const size_t numberOfBytes_, void (*const transferComplete_) (void)) {

    // Store arguments
    csPin = csPin_;
#ifdef PRINT_TRANSFERS
    data = (uint8_t*) data_;
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
#ifdef SPI2_CS_ACTIVE_HIGH
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
    EVIC_SourceStatusClear(INT_SOURCE_DMA1); // clear interrupt flag first because transfer complete callback may start a new transfer
    if (csPin != GPIO_PIN_NONE) {
#ifdef SPI2_CS_ACTIVE_HIGH
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
bool Spi2DmaTransferInProgress(void) {
    return DCH1CONbits.CHEN == 1; // if RX DMA channel interrupt enabled
}

//------------------------------------------------------------------------------
// End of file
