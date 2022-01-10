/**
 * @file UsbMsdIsAttached.h
 * @author Seb Madgwick
 * @brief Functions to allow an application to prevent the detection of an
 * attached SD card by MPLAB Harmony.
 */

#ifndef USB_MSD_IS_ATTACHED_H
#define USB_MSD_IS_ATTACHED_H

//------------------------------------------------------------------------------
// Includes

#include "config/default/driver/sdspi/drv_sdspi.h"
#include <stdbool.h>

//------------------------------------------------------------------------------
// Function declarations

bool UsbMsdIsAttachedGet(const DRV_HANDLE handle);
void UsbMsdIsAttachedSet(const bool isAttached);

#endif

//------------------------------------------------------------------------------
// End of file
