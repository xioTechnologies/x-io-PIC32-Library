/**
 * @file UsbMsd.h
 * @author Seb Madgwick
 * @brief Application interface for USB MSD functionality using MPLAB Harmony.
 */

#ifndef USB_MSD_H
#define USB_MSD_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>

//------------------------------------------------------------------------------
// Function declarations

void UsbMsdTasks(void);
bool UsbMsdHostConnected(void);

#endif

//------------------------------------------------------------------------------
// End of file
