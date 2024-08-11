/**
 * @file UsbMsd.c
 * @author Seb Madgwick
 * @brief Application interface for USB MSD functionality using MPLAB Harmony.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "UsbMsd.h"

//------------------------------------------------------------------------------
// Function declarations

static void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void * pEventData, uintptr_t context);

//------------------------------------------------------------------------------
// Variables

static USB_DEVICE_HANDLE usbDeviceHandle = USB_DEVICE_HANDLE_INVALID;
static bool isHostConnected;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Module tasks.  This function should be called repeatedly within the
 * main program loop.
 */
void UsbMsdTasks(void) {
    if (usbDeviceHandle == USB_DEVICE_HANDLE_INVALID) {
        usbDeviceHandle = USB_DEVICE_Open(USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE);
        if (usbDeviceHandle != USB_DEVICE_HANDLE_INVALID) {
            USB_DEVICE_EventHandlerSet(usbDeviceHandle, APP_USBDeviceEventHandler, 0);
        }
    }
}

/**
 * @breif USB device event handler based on MPLAB Harmony examples.
 */
static void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void * pEventData, uintptr_t context) {
    switch (event) {
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_SUSPENDED:
        case USB_DEVICE_EVENT_DECONFIGURED:
            isHostConnected = false;
            break;
        case USB_DEVICE_EVENT_CONFIGURED:
            if (((USB_DEVICE_EVENT_DATA_CONFIGURED *) pEventData)->configurationValue == 1) {
                isHostConnected = true;
            }
            break;
        case USB_DEVICE_EVENT_POWER_DETECTED:
            USB_DEVICE_Attach(usbDeviceHandle);
            break;
        case USB_DEVICE_EVENT_POWER_REMOVED:
            USB_DEVICE_Detach(usbDeviceHandle);
			isHostConnected = false;
            break;
        default:
            break;
    }
}

/**
 * @brief Returns true if the USB host is connected.
 * @return True if the USB host is connected.
 */
bool UsbMsdHostConnected(void) {
    return isHostConnected;
}

//------------------------------------------------------------------------------
// End of file
