/**
 * @file PrintResetReason.c
 * @author Seb Madgwick
 * @brief Prints the reset reason.
 */

//------------------------------------------------------------------------------
// Includes

#include "peripheral/reset/plib_reset.h"
#include <stdio.h> // printf

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Prints the reset reason.
 */
void PrintResetReason() {

    // Get reset reason
    const RESET_REASON resetReason = PLIB_RESET_ReasonGet(RESET_ID_0);
    PLIB_RESET_ReasonClear(RESET_ID_0, RESET_REASON_ALL);

    // Print reset reasons common to all devices
    if ((resetReason & RESET_REASON_NONE) != 0) {
        printf("RESET_REASON_NONE\r\n");
    }
    if ((resetReason & RESET_REASON_POWERON) != 0) {
        printf("RESET_REASON_POWERON\r\n");
    }
    if ((resetReason & RESET_REASON_BROWNOUT) != 0) {
        printf("RESET_REASON_BROWNOUT\r\n");
    }
    if ((resetReason & RESET_REASON_WDT_TIMEOUT) != 0) {
        printf("RESET_REASON_WDT_TIMEOUT\r\n");
    }
    if ((resetReason & RESET_REASON_SOFTWARE) != 0) {
        printf("RESET_REASON_SOFTWARE\r\n");
    }
    if ((resetReason & RESET_REASON_MCLR) != 0) {
        printf("RESET_REASON_MCLR\r\n");
    }

    // Print device-specific reset reason
    const RESET_REASON mask = RESET_REASON_POWERON | RESET_REASON_BROWNOUT | RESET_REASON_WDT_TIMEOUT | RESET_REASON_SOFTWARE | RESET_REASON_MCLR;
    if ((resetReason & ~mask) != 0) {
        printf("RESET_REASON_%08X\r\n", resetReason);
    }
}

//------------------------------------------------------------------------------
// End of file
