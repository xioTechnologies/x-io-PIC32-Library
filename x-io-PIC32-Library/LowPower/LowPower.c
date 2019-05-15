/**
 * @file LowPower.c
 * @author Seb Madgwick
 * @brief Low power modes for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "peripheral/devcon/plib_devcon.h"
#include "peripheral/osc/plib_osc.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Enables idle mode.
 */
void LowPowerIdle() {
    PLIB_DEVCON_SystemUnlock(DEVCON_ID_0);
    PLIB_OSC_OnWaitActionSet(OSC_ID_0, OSC_ON_WAIT_IDLE);
    PLIB_DEVCON_SystemLock(DEVCON_ID_0);
    asm("WAIT");
}

/**
 * @brief Enables sleep mode.
 */
void LowPowerSleep() {
    PLIB_DEVCON_SystemUnlock(DEVCON_ID_0);
    PLIB_OSC_OnWaitActionSet(OSC_ID_0, OSC_ON_WAIT_SLEEP);
    PLIB_DEVCON_SystemLock(DEVCON_ID_0);
    asm("WAIT");
}

//------------------------------------------------------------------------------
// End of file
