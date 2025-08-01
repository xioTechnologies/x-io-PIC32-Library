/**
 * @file UsbCdcMsd.c
 * @author Seb Madgwick
 * @brief Application interface for a USB CDC device using MPLAB Harmony.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "definitions.h"
#include "UsbCdc.h"

//------------------------------------------------------------------------------
// Function declarations

static void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void * eventData, uintptr_t context);
static void APP_USBDeviceCDCEventHandler(USB_DEVICE_CDC_INDEX instanceIndex, USB_DEVICE_CDC_EVENT event, void* pData, uintptr_t context);
static void ReadTasks(void);
static void WriteTasks(void);

//------------------------------------------------------------------------------
// Variables

static volatile USB_DEVICE_HANDLE usbDeviceHandle = USB_DEVICE_HANDLE_INVALID;
static volatile bool hostConnected;
static volatile bool portOpen;
static volatile uint8_t __attribute__((coherent)) readRequestData[512]; // must be declared __attribute__((coherent)) for PIC32MZ devices
static volatile bool readInProgress;
static volatile bool writeInProgress;
static uint8_t readData[USB_CDC_READ_BUFFER_SIZE];
static Fifo readFifo = {.data = readData, .dataSize = sizeof (readData)};
static uint8_t writeData[USB_CDC_WRITE_BUFFER_SIZE];
static Fifo writeFifo = {.data = writeData, .dataSize = sizeof (writeData)};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Module tasks. This function should be called repeatedly within the
 * main program loop.
 */
void UsbCdcTasks(void) {

    // Open USB device driver
    if (usbDeviceHandle == USB_DEVICE_HANDLE_INVALID) {
        usbDeviceHandle = USB_DEVICE_Open(USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE);
        if (usbDeviceHandle != USB_DEVICE_HANDLE_INVALID) {
            USB_DEVICE_EventHandlerSet(usbDeviceHandle, APP_USBDeviceEventHandler, 0);
        }
    }

    // CDC read/write tasks
    if (hostConnected) {
        ReadTasks();
        WriteTasks();
    }
}

/**
 * @brief USB device event handler based on MPLAB Harmony examples.
 */
static void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void * eventData, uintptr_t context) {
    switch (event) {
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_SUSPENDED:
        case USB_DEVICE_EVENT_DECONFIGURED:
            hostConnected = false;
            portOpen = false;
            readInProgress = false;
            writeInProgress = false;
            break;
        case USB_DEVICE_EVENT_CONFIGURED:
            if (((USB_DEVICE_EVENT_DATA_CONFIGURED *) eventData)->configurationValue == 1) {
                USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0, APP_USBDeviceCDCEventHandler, (uintptr_t) NULL);
                hostConnected = true;
            }
            break;
        case USB_DEVICE_EVENT_POWER_DETECTED:
            USB_DEVICE_Attach(usbDeviceHandle);
            break;
        case USB_DEVICE_EVENT_POWER_REMOVED:
            USB_DEVICE_Detach(usbDeviceHandle);
            hostConnected = false;
            portOpen = false;
            readInProgress = false;
            writeInProgress = false;
            break;
        default:
            break;
    }
}

/**
 * @brief USB device CDC event handler based on MPLAB Harmony examples.
 */
static void APP_USBDeviceCDCEventHandler(USB_DEVICE_CDC_INDEX index, USB_DEVICE_CDC_EVENT event, void * pData, uintptr_t userData) {
    static USB_CDC_LINE_CODING usbCdcLineCoding = {
        .bDataBits = 8,
        .dwDTERate = 115000,
        .bCharFormat = 0,
        .bParityType = 0
    };
    switch (event) {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:
            USB_DEVICE_ControlSend(usbDeviceHandle, (uint8_t *) & usbCdcLineCoding, sizeof (usbCdcLineCoding));
            break;
        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:
            USB_DEVICE_ControlReceive(usbDeviceHandle, (uint8_t *) & usbCdcLineCoding, sizeof (usbCdcLineCoding));
            break;
        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:
            portOpen = ((USB_CDC_CONTROL_LINE_STATE *) pData)->dtr == 1;
            USB_DEVICE_ControlStatus(usbDeviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_SEND_BREAK:
            USB_DEVICE_ControlStatus(usbDeviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:
            if (readInProgress) { // prevent unexpected read event for PIC32MZ devices when host reconnected
                const size_t numberOfBytes = ((USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE*) pData)->length;
                FifoWrite(&readFifo, (void*) readRequestData, numberOfBytes);
                readInProgress = false;
            }
            break;
        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:
            USB_DEVICE_ControlStatus(usbDeviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:
            writeInProgress = false;
            break;
        default:
            break;
    }
}

/**
 * @brief Read tasks.
 */
static void ReadTasks(void) {

    // Do nothing if read in progress
    if (readInProgress) {
        return;
    }

    // Schedule read
    readInProgress = true;
    static USB_DEVICE_CDC_TRANSFER_HANDLE usbDeviceCdcTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    const USB_DEVICE_CDC_RESULT usbDeviceCdcResult = USB_DEVICE_CDC_Read(USB_DEVICE_CDC_INDEX_0, &usbDeviceCdcTransferHandle, (void*) readRequestData, sizeof (readRequestData));
    if (usbDeviceCdcResult != USB_DEVICE_CDC_RESULT_OK) {
        readInProgress = false;
        return;
    }
}

/**
 * @brief Write tasks.
 */
static void WriteTasks(void) {

    // Do nothing if write in progress
    if (writeInProgress) {
        return;
    }

    // Do nothing if no data available
    if (FifoAvailableRead(&writeFifo) == 0) {
        return;
    }

    // Copy data to buffer
    static uint8_t __attribute__((coherent)) buffer[1024]; // must be declared __attribute__((coherent)) for PIC32MZ devices
    const size_t numberOfBytes = FifoRead(&writeFifo, buffer, sizeof (buffer));

    // Schedule write
    writeInProgress = true;
    static USB_DEVICE_CDC_TRANSFER_HANDLE usbDeviceCdcTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    const USB_DEVICE_CDC_RESULT usbDeviceCdcResult = USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0, &usbDeviceCdcTransferHandle, buffer, numberOfBytes, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
    if (usbDeviceCdcResult != USB_DEVICE_CDC_RESULT_OK) {
        writeInProgress = false;
        return;
    }
}

/**
 * @brief Returns true if the USB host is connected.
 * @return True if the USB host is connected.
 */
bool UsbCdcHostConnected(void) {
    return hostConnected;
}

/**
 * @brief Returns true if the port is open.
 * @return True if the port is open.
 */
bool UsbCdcPortOpen(void) {
    return portOpen;
}

/**
 * @brief Returns the number of bytes available in the read buffer.
 * @return Number of bytes available in the read buffer.
 */
size_t UsbCdcAvailableRead(void) {
    return FifoAvailableRead(&readFifo);
}

/**
 * @brief Reads data from the read buffer.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes.
 * @return Number of bytes read.
 */
size_t UsbCdcRead(void* const destination, size_t numberOfBytes) {
    return FifoRead(&readFifo, destination, numberOfBytes);
}

/**
 * @brief Reads a byte from the read buffer. This function must only be called
 * if there are bytes available in the read buffer.
 * @return Byte.
 */
uint8_t UsbCdcReadByte(void) {
    return FifoReadByte(&readFifo);
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t UsbCdcAvailableWrite(void) {
    return FifoAvailableWrite(&writeFifo);
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @return Result.
 */
FifoResult UsbCdcWrite(const void* const data, const size_t numberOfBytes) {
    return FifoWrite(&writeFifo, data, numberOfBytes);
}

/**
 * @brief Writes a byte to the write buffer.
 * @param byte Byte.
 * @return Result.
 */
FifoResult UsbCdcWriteByte(const uint8_t byte) {
    return FifoWriteByte(&writeFifo, byte);
}

//------------------------------------------------------------------------------
// End of file
