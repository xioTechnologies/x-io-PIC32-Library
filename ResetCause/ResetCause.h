/**
 * @file ResetCause.h
 * @author Seb Madgwick
 * @brief Indicates the cause of a device reset.
 */

#ifndef RESET_CAUSE_H
#define RESET_CAUSE_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"

//------------------------------------------------------------------------------
// Function declarations

RCON_RESET_CAUSE ResetCauseGet(void);
void ResetCausePrint(const RCON_RESET_CAUSE cause);

#endif

//------------------------------------------------------------------------------
// End of file
