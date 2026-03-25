/**
 * @file UsbMsd.h
 * @author Seb Madgwick
 * @brief Wrapper for the MPLAB Harmony USB MSD device driver.
 */

#ifndef USB_MSD_H_
#define USB_MSD_H_

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>

//------------------------------------------------------------------------------
// Function declarations

void UsbMsdTasks(void);
bool UsbMsdVbusValid(void);
bool UsbMsdHostConnected(void);

#endif

//------------------------------------------------------------------------------
// End of file
