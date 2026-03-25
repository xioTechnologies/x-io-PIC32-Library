/**
 * @file UsbMsdAttached.c
 * @author Seb Madgwick
 * @brief Overrides DRV_SDSPI_IsAttached to allow the application to prevent
 * SD card detection by the USB MSD stack.
 */

//------------------------------------------------------------------------------
// Includes

#include "UsbMsdAttached.h"

//------------------------------------------------------------------------------
// Variables

static bool enabled;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Returns true if the SD card is attached and the application has not
 * prevented detection. This function should be used instead of
 * DRV_SDSPI_IsAttached in usb_device_init_data.c.
 * @param handle Handle
 * @return True if the SD card is attached and the application has not prevented
 * detection.
 */
bool UsbMsdAttachedGet(const DRV_HANDLE handle) {
    return enabled && (DRV_SDSPI_IsAttached(handle));
}

/**
 * @brief Sets the application override for SD card detection.
 * @param enabled_ True to enable detection.
 */
void UsbMsdAttachedSet(const bool enabled_) {
    enabled = enabled_;
}

//------------------------------------------------------------------------------
// End of file
