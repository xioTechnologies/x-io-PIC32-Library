/**
 * @file ResetReason.c
 * @author Seb Madgwick
 * @brief Determining the reason for device reset.
 */

//------------------------------------------------------------------------------
// Includes

#include "ResetReason.h"
#include <stdio.h> // printf
#include <xc.h>

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Gets the reset reason.  This function can only be called once.
 * @return Reset reason.
 */
ResetReasons ResetReasonGet() {
    const ResetReasons reasons = RCON;
    RCON &= ~ResetReasonsAll;
    return reasons;
}

/**
 * @brief Prints the reset reason.
 * @return Reset reason.
 */
void ResetReasonPrint(const ResetReasons reasons) {
    if ((reasons & ResetReasonsPowerOn) != 0) {
        printf("Power on reset\r\n");
    }
    if ((reasons & ResetReasonsBrownout) != 0) {
        printf("Brownout reset\r\n");
    }
    if ((reasons & ResetReasonsWatchdogTimer) != 0) {
        printf("Watchdog timer reset\r\n");
    }
    if ((reasons & ResetReasonsDeadmanTimer) != 0) {
        printf("Deadman timer reset\r\n");
    }
    if ((reasons & ResetReasonsSoftware) != 0) {
        printf("Software reset\r\n");
    }
    if ((reasons & ResetReasonsExternal) != 0) {
        printf("External reset\r\n");
    }
    if ((reasons & ResetReasonsConfigurationMismatch) != 0) {
        printf("Configuration mismatch reset\r\n");
    }
    if ((reasons & ResetReasonsAll) == 0) {
        printf("Unknown reset\r\n");
    }
}

//------------------------------------------------------------------------------
// End of file
