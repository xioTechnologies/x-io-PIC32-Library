/**
 * @file MsdMediaIsAttached.c
 * @author Seb Madgwick
 * @brief Functions to allow an application to prevent the detection of an
 * attached SD card by MPLAB Harmony.
 */

//------------------------------------------------------------------------------
// Includes

#include "driver/sdcard/drv_sdcard.h"
#include "MsdMediaIsAttached.h"

//------------------------------------------------------------------------------
// Variables

static bool applicationIsAttachedState;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Returns true if the SD card is attached and the application has not
 * prevented the USB stack from detecting this.
 * @return True if the SD card is attached and the application has not prevented
 * the USB stack from detecting this.
 */
bool MsdMediaIsAttachedGet() {
    return (applicationIsAttachedState == true) && (DRV_SDCARD_IsAttached(DRV_SDCARD_INDEX_0) == true);
}

/**
 * @brief Prevent or allow the USB stack from detecting an attached SD card.
 * @param isAttached True if the application allows detection of an attached SD
 * card.
 */
void MsdMediaIsAttachedSet(const bool isAttached) {
    applicationIsAttachedState = isAttached;
}

//------------------------------------------------------------------------------
// End of file
