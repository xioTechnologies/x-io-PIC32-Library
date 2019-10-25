/**
 * @file Spi3Dma.c
 * @author Seb Madgwick
 * @brief SPI driver using DMA for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "Spi3Dma.h"
#include "system_definitions.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Variables

static void (*transferCompleteCallback)();

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param settings Settings.
 */
void Spi3DmaInitialise(const SpiSettings * const settings) {

    // Ensure default register states
    Spi3DmaDisable();

    // Configure SPI
    SPI3CONbits.MSTEN = 1; // Master mode
    SPI3CONbits.CKP = settings->clockPolarity;
    SPI3CONbits.CKE = settings->clockPhase;
    SPI3BRG = SpiCalculateSpixbrg(settings->clockFrequency);
    SPI3CONbits.ON = 1;

    // Enable DMA
    DMACONbits.ON = 1;

    // Configure TX DMA channel
    DCH0ECONbits.CHSIRQ = _SPI3_TX_VECTOR;
    DCH0ECONbits.SIRQEN = 1; // Start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH0DSA = KVA_TO_PA(&SPI3BUF); // destination address
    DCH0DSIZ = 1; // destination size
    DCH0CSIZ = 1; // transfers per event

    // Configure RX DMA channel
    DCH1ECONbits.CHSIRQ = _SPI3_RX_VECTOR;
    DCH1ECONbits.SIRQEN = 1; // Start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH1SSA = KVA_TO_PA(&SPI3BUF); // source address
    DCH1SSIZ = 1; // source size
    DCH1CSIZ = 1; // transfers per event
    DCH1INTbits.CHBCIE = 1; // Channel Block Transfer Complete Interrupt Enable bit

    // Configure RX DMA channel interrupt
    SYS_INT_VectorPrioritySet(_DMA1_VECTOR, INT_PRIORITY_LEVEL5);
    SYS_INT_SourceStatusClear(INT_SOURCE_DMA_1);
    SYS_INT_SourceEnable(INT_SOURCE_DMA_1);
}

/**
 * @brief Disables the module.
 */
void Spi3DmaDisable() {

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
    SYS_INT_SourceDisable(INT_SOURCE_DMA_1);
    SYS_INT_SourceStatusClear(INT_SOURCE_DMA_1);
}

/**
 * @brief Sets the transfer complete callback function.  This callback function
 * will be called from within an interrupt once the transfer is complete.
 * @param transferComplete Transfer complete callback function.
 */
void Spi3DmaSetCallback(void (*transferComplete)()) {
    SYS_INT_SourceDisable(INT_SOURCE_DMA_1);
    transferCompleteCallback = transferComplete;
    SYS_INT_SourceEnable(INT_SOURCE_DMA_1);
}

/**
 * @brief Transfers data.  The data being transmitted will be overwritten with
 * the received data.  The data must be declared __attribute__((coherent))
 * for PIC32MZ devices.  This function must not be called while a transfer is in
 * progress.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void Spi3DmaTransfer(void* const data, const size_t numberOfBytes) {

    // Configure TX DMA channel
    DCH0SSA = KVA_TO_PA(data); // source address
    DCH0SSIZ = numberOfBytes; // source size

    // Configure RX DMA channel
    DCH1DSA = KVA_TO_PA(data); // destination address
    DCH1DSIZ = numberOfBytes; // destination size

    // Begin transfer
    DCH1INTbits.CHBCIF = 0; // clear RX DMA channel interrupt flag
    DCH1CONbits.CHEN = 1; // enable RX DMA channel
    DCH0CONbits.CHEN = 1; // enable TX DMA channel to begin transfer
}

/**
 * @brief RX DMA channel interrupt.
 */
void __ISR(_DMA1_VECTOR) Dma1Interrupt() {
    SYS_INT_SourceStatusClear(INT_SOURCE_DMA_1); // clear interrupt flag first because callback function may start new transfer
    if (transferCompleteCallback != NULL) {
        transferCompleteCallback();
    }
}

/**
 * @brief Returns true while the transfer is in progress.
 * @return True while the transfer is in progress.
 */
bool Spi3DmaIsTransferInProgress() {
    return (DCH1CONbits.CHEN == 1) && (DCH1INTbits.CHDDIF == 0); // if RX DMA channel interrupt enabled and flag not set
}

//------------------------------------------------------------------------------
// End of file
