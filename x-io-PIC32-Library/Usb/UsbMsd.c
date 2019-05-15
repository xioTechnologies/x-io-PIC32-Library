/**
 * @file UsbMsd.c
 * @author Seb Madgwick
 * @brief Application interface for USB MSD functionality using MPLAB Harmony.
 */

//------------------------------------------------------------------------------
// Includes

#include "system_definitions.h"
#include "UsbMsd.h"

//------------------------------------------------------------------------------
// Function prototypes

static void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void* eventData, uintptr_t context);

//------------------------------------------------------------------------------
// Variables

static USB_DEVICE_HANDLE usbDeviceHandle = USB_DEVICE_HANDLE_INVALID;
static bool isConfigured;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Module tasks.  This function should be called repeatedly within the
 * main program loop.
 */
void UsbMsdTasks() {
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
static void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void* eventData, uintptr_t context) {
    switch (event) {
        case USB_DEVICE_EVENT_RESET: // fall-through to next case
        case USB_DEVICE_EVENT_DECONFIGURED:
            isConfigured = false;
            break;
        case USB_DEVICE_EVENT_CONFIGURED:
            if (((USB_DEVICE_EVENT_DATA_CONFIGURED *) eventData)->configurationValue == 1) {
                isConfigured = true;
            }
            break;
        case USB_DEVICE_EVENT_POWER_DETECTED:
            USB_DEVICE_Attach(usbDeviceHandle);
            break;
        case USB_DEVICE_EVENT_POWER_REMOVED:
            USB_DEVICE_Detach(usbDeviceHandle);
            isConfigured = false;
            break;
        default:
            break;
    }
}

/**
 * @brief Returns true if the USB host is connected.
 * @return True if the USB host is connected.
 */
bool UsbMsdHostIsConnected() {
    return isConfigured;
}

//------------------------------------------------------------------------------
// End of file
