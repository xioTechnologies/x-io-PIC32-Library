/**
 * @file UsbMsdAttached.c
 * @author Seb Madgwick
 * @brief Functions to allow an application to prevent the detection of an
 * attached SD card by MPLAB Harmony.
 */

//------------------------------------------------------------------------------
// Includes

#include "UsbMsdAttached.h"

//------------------------------------------------------------------------------
// Variables

static bool isAttached;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Returns true if the SD card is attached and the application has not
 * prevented the USB stack from detecting this. This function should be used
 * instead of DRV_SDSPI_IsAttached in usb_device_init_data.c.
 * @param handle Handle
 * @return True if the SD card is attached and the application has not prevented
 * the USB stack from detecting this.
 */
bool UsbMsdAttachedGet(const DRV_HANDLE handle) {
    return isAttached && (DRV_SDSPI_IsAttached(handle));
}

/**
 * @brief Prevent or allow the USB stack from detecting an attached SD card.
 * @param isAttached_ True if the application allows detection of an attached SD
 * card.
 */
void UsbMsdAttachedSet(const bool isAttached_) {
    isAttached = isAttached_;
}

//------------------------------------------------------------------------------
// End of file
