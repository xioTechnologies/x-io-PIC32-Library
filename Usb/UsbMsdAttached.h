/**
 * @file UsbMsdAttached.h
 * @author Seb Madgwick
 * @brief Overrides DRV_SDSPI_IsAttached to allow the application to prevent
 * SD card detection by the USB MSD stack.
 */

#ifndef USB_MSD_ATTACHED_H
#define USB_MSD_ATTACHED_H

//------------------------------------------------------------------------------
// Includes

#include "config/default/driver/sdspi/drv_sdspi.h"
#include <stdbool.h>

//------------------------------------------------------------------------------
// Function declarations

bool UsbMsdAttachedGet(const DRV_HANDLE handle);
void UsbMsdAttachedSet(const bool isAttached);

#endif

//------------------------------------------------------------------------------
// End of file
