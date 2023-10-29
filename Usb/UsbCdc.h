/**
 * @file UsbCdc.h
 * @author Seb Madgwick
 * @brief Application interface for a USB CDC device using MPLAB Harmony.
 */

#ifndef USB_CDC_H
#define USB_CDC_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void UsbCdcTasks(void);
bool UsbCdcIsHostConnected(void);
size_t UsbCdcGetReadAvailable(void);
size_t UsbCdcRead(void* const destination, size_t numberOfBytes);
uint8_t UsbCdcReadByte(void);
size_t UsbCdcGetWriteAvailable(void);
void UsbCdcWrite(const void* const data, const size_t numberOfBytes);
void UsbCdcWriteByte(const uint8_t byte);

#endif

//------------------------------------------------------------------------------
// End of file
