/**
 * @file ResetReason.h
 * @author Seb Madgwick
 * @brief Determining the reason for device reset.
 */

#ifndef RESET_REASON_H
#define RESET_REASON_H

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Reset reasons.  Reset status bits in the RCON register.
 */
typedef enum {
    ResetReasonsPowerOn = (1 << 0),
    ResetReasonsBrownout = (1 << 1),
    ResetReasonsWatchdogTimer = (1 << 4),
    ResetReasonsDeadmanTimer = (1 << 5),
    ResetReasonsSoftware = (1 << 6),
    ResetReasonsExternal = (1 << 7),
    ResetReasonsConfigurationMismatch = (1 << 9),
    ResetReasonsAll = ResetReasonsPowerOn | ResetReasonsBrownout | ResetReasonsWatchdogTimer | ResetReasonsDeadmanTimer | ResetReasonsSoftware | ResetReasonsExternal | ResetReasonsConfigurationMismatch,
} ResetReasons;

//------------------------------------------------------------------------------
// Function declarations

ResetReasons ResetReasonGet();
void ResetReasonPrint(const ResetReasons reasons);

#endif

//------------------------------------------------------------------------------
// End of file
