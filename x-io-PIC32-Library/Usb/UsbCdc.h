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
// Function prototypes

void UsbCdcTasks();
bool UsbCdcIsHostConnected();
size_t UsbCdcGetReadAvailable();
size_t UsbCdcRead(void* const destination, size_t numberOfBytes);
uint8_t UsbCdcReadByte();
size_t UsbCdcGetWriteAvailable();
void UsbCdcWrite(const void* const data, const size_t numberOfBytes);
void UsbCdcWriteByte(const uint8_t byte);

#endif

//------------------------------------------------------------------------------
// End of file
