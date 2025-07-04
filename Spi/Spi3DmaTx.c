/**
 * @file Spi3DmaTx.c
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices. DMA used for TX only.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "Spi3DmaTx.h"
#include <stdio.h>
#include "sys/kmem.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Uncomment this line to enable printing of transfers.
 */
//#define PRINT_TRANSFERS

//------------------------------------------------------------------------------
// Variables

static GPIO_PIN csPin;
static void (*transferComplete)(void);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 */
void Spi3DmaTxInitialise(const SpiSettings * const settings) {

    // Ensure default register states
    Spi3DmaTxDeinitialise();

    // Configure SPI
    SPI3CONbits.MSTEN = 1; // host mode
    SPI3CONbits.ENHBUF = 1; // enhanced Buffer mode is enabled
    SPI3CONbits.SMP = 1; // input data sampled at end of data output time
    SPI3CONbits.CKP = settings->clockPolarity;
    SPI3CONbits.CKE = settings->clockPhase;
    SPI3CONbits.STXISEL = 0b11; // interrupt is generated when the buffer is not full (has one or more empty elements)
    SPI3CONbits.SRXISEL = 0b01; // interrupt is generated when the buffer is not empty
    SPI3BRG = SpiCalculateSpixbrg(settings->clockFrequency);
    SPI3CONbits.ON = 1;

    // Enable DMA
    DMACONbits.ON = 1;

    // Configure TX DMA channel
#ifdef _SPI3_TX_IRQ
    DCH0ECONbits.CHSIRQ = _SPI3_TX_IRQ;
#else
    DCH0ECONbits.CHSIRQ = _SPI3_TX_VECTOR;
#endif
    DCH0ECONbits.SIRQEN = 1; // start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH0DSA = KVA_TO_PA(&SPI3BUF); // destination address
    DCH0DSIZ = 1; // destination size
    DCH0CSIZ = 1; // transfers per event
    DCH0INTbits.CHBCIE = 1; // channel Block Transfer Complete Interrupt Enable bit

    // Enable interrupts
    EVIC_SourceEnable(INT_SOURCE_DMA0);
}

/**
 * @brief Deinitialises the module.
 */
void Spi3DmaTxDeinitialise(void) {

    // Disable SPI and restore default register states
    SPI3CON = 0;
    SPI3CON2 = 0;
    SPI3STAT = 0;
    SPI3BRG = 0;

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

    // Disable interrupt
    EVIC_SourceDisable(INT_SOURCE_DMA0);
    EVIC_SourceStatusClear(INT_SOURCE_DMA0);
}

/**
 * @brief Transfers data. Received data will be discarded. The data must be
 * declared __attribute__((coherent)) for PIC32MZ devices. This function must
 * not be called while a transfer is in progress. The transfer complete callback
 * will be called from within an interrupt once the transfer is complete.
 * @param csPin_ CS pin.
 * @param data_ Data.
 * @param numberOfBytes_ Number of bytes.
 * @param transferComplete_ Transfer complete callback.
 */
void Spi3DmaTxTransfer(const GPIO_PIN csPin_, const void* const data, const size_t numberOfBytes, void (*transferComplete_)(void)) {

    // Store arguments
    csPin = csPin_;
    transferComplete = transferComplete_;

    // Print
#ifdef PRINT_TRANSFERS
    SpiPrintTransfer(csPin, data, numberOfBytes);
#endif

    // Configure TX DMA channel
    DCH0SSA = KVA_TO_PA(data); // source address
    DCH0SSIZ = numberOfBytes; // source size

    // Begin transfer
    if (csPin != GPIO_PIN_NONE) {
        GPIO_PinClear(csPin);
    }
    DCH0INTbits.CHBCIF = 0; // clear TX DMA channel interrupt flag
    DCH0CONbits.CHEN = 1; // enable TX DMA channel to begin transfer
}

/**
 * @brief DMA interrupt handler. This function should be called by the ISR
 * implementation generated by MPLAB Harmony.
 */
void Dma0InterruptHandler(void) {
    EVIC_SourceStatusClear(INT_SOURCE_DMA0); // clear interrupt flag first because callback may start new transfer
    if (csPin != GPIO_PIN_NONE) {
        GPIO_PinSet(csPin);
    }
    if (transferComplete != NULL) {
        transferComplete();
    }
}

/**
 * @brief Returns true while the transfer is in progress.
 * @return True while the transfer is in progress.
 */
bool Spi3DmaTxTransferInProgress(void) {
    return DCH0CONbits.CHEN == 1; // if TX DMA channel interrupt enabled
}

//------------------------------------------------------------------------------
// End of file
