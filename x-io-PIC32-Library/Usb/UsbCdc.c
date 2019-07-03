/**
 * @file UsbCdcMsd.c
 * @author Seb Madgwick
 * @brief Application interface for a USB CDC device using MPLAB Harmony.
 */

//------------------------------------------------------------------------------
// Includes

#include "CircularBuffer.h"
#include "system_definitions.h"
#include "UsbCdc.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Read and write buffers size.  Must be a 2^n number, e.g. 256, 512,
 * 1024, 2048, 4096, etc.
 */
#define READ_WRITE_BUFFER_SIZE (4096)

/**
 * @brief Read and write buffers index mask.  This value is bitwise anded with
 * buffer indexes for fast overflow operations.
 */
#define READ_WRITE_BUFFER_INDEX_BIT_MASK (READ_WRITE_BUFFER_SIZE - 1)

//------------------------------------------------------------------------------
// Function prototypes

static void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void * eventData, uintptr_t context);
static void APP_USBDeviceCDCEventHandler(USB_DEVICE_CDC_INDEX instanceIndex, USB_DEVICE_CDC_EVENT event, void* pData, uintptr_t context);
static void ReadTasks();
static void WriteTasks();

//------------------------------------------------------------------------------
// Variables

static USB_DEVICE_HANDLE usbDeviceHandle = USB_DEVICE_HANDLE_INVALID;
static bool isConfigured;
#if defined __PIC32MZ__
static uint8_t __attribute__((coherent, aligned(16))) readRequestData[512];
#else
static uint8_t readRequestData[64];
#endif
static bool readInProgress;
static bool writeInProgress;
static uint8_t readBuffer[READ_WRITE_BUFFER_SIZE];
static uint32_t readBufferWriteIndex;
static uint32_t readBufferReadIndex;
static uint8_t writeBuffer[READ_WRITE_BUFFER_SIZE];
static uint32_t writeBufferWriteIndex;
static uint32_t writeBufferReadIndex;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Module tasks.  This function should be called repeatedly within the
 * main program loop.
 */
void UsbCdcTasks() {

    // Open USB device driver
    if (usbDeviceHandle == USB_DEVICE_HANDLE_INVALID) {
        usbDeviceHandle = USB_DEVICE_Open(USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE);
        if (usbDeviceHandle != USB_DEVICE_HANDLE_INVALID) {
            USB_DEVICE_EventHandlerSet(usbDeviceHandle, APP_USBDeviceEventHandler, 0);
        }
    }

    // CDC read/write tasks
    if (isConfigured == true) {
        ReadTasks();
        WriteTasks();
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
                USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0, APP_USBDeviceCDCEventHandler, (uintptr_t) NULL);
                isConfigured = true;
                readInProgress = false;
                writeInProgress = false;
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
 * @breif USB device CDC event handler based on MPLAB Harmony examples.
 */
static void APP_USBDeviceCDCEventHandler(USB_DEVICE_CDC_INDEX instanceIndex, USB_DEVICE_CDC_EVENT event, void* pData, uintptr_t context) {
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
            USB_DEVICE_ControlStatus(usbDeviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_SEND_BREAK:
            USB_DEVICE_ControlStatus(usbDeviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:
        {
            const size_t numberOfBytes = ((USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE*) pData)->length;
            readBufferWriteIndex &= READ_WRITE_BUFFER_INDEX_BIT_MASK;
            CircularBufferWrite(readBuffer, READ_WRITE_BUFFER_SIZE, &readBufferWriteIndex, readRequestData, numberOfBytes);
            readInProgress = false;
            break;
        }
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
static void ReadTasks() {

    // Do nothing if read in progress
    if (readInProgress == true) {
        return;
    }

    // Schedule read
    readInProgress = true;
    static USB_DEVICE_CDC_TRANSFER_HANDLE usbDeviceCdcTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    const USB_DEVICE_CDC_RESULT usbDeviceCdcResult = USB_DEVICE_CDC_Read(USB_DEVICE_CDC_INDEX_0, &usbDeviceCdcTransferHandle, readRequestData, sizeof (readRequestData));
    if (usbDeviceCdcResult != USB_DEVICE_CDC_RESULT_OK) {
        readInProgress = false;
        return;
    }
}

/**
 * @brief Write tasks.
 */
static void WriteTasks() {

    // Do nothing if write in progress
    if (writeInProgress == true) {
        return;
    }

    // Do nothing if no data available
    size_t numberOfBytes = (writeBufferWriteIndex - writeBufferReadIndex) & READ_WRITE_BUFFER_INDEX_BIT_MASK;
    if (numberOfBytes == 0) {
        return;
    }

    // Copy data to buffer
#if defined __PIC32MZ__
    static uint8_t __attribute__((coherent, aligned(16))) buffer[1024];
#else
    static uint8_t buffer[1024];
#endif
    if (numberOfBytes > sizeof (buffer)) {
        numberOfBytes = sizeof (buffer);
    }
    uint32_t writeBufferReadIndexCache = writeBufferReadIndex & READ_WRITE_BUFFER_INDEX_BIT_MASK;
    CircularBufferRead(writeBuffer, READ_WRITE_BUFFER_SIZE, &writeBufferReadIndexCache, buffer, numberOfBytes);

    // Schedule write
    writeInProgress = true;
    static USB_DEVICE_CDC_TRANSFER_HANDLE usbDeviceCdcTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    const USB_DEVICE_CDC_RESULT usbDeviceCdcResult = USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0, &usbDeviceCdcTransferHandle, buffer, numberOfBytes, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
    if (usbDeviceCdcResult != USB_DEVICE_CDC_RESULT_OK) {
        writeInProgress = false;
        return;
    }
    writeBufferReadIndex = writeBufferReadIndexCache;
}

/**
 * @brief Returns true if the USB host is connected.
 * @return True if the USB host is connected.
 */
bool UsbCdcIsHostConnected() {
    return isConfigured;
}

/**
 * @brief Returns the number of bytes available in the read buffer.
 * @return Number of bytes available in the read buffer.
 */
size_t UsbCdcGetReadAvailable() {
    return (readBufferWriteIndex - readBufferReadIndex) & READ_WRITE_BUFFER_INDEX_BIT_MASK;
}

/**
 * @brief Reads data from the read buffer.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes.
 * @return Number of bytes read.
 */
size_t UsbCdcRead(void* const destination, size_t numberOfBytes) {

    // Limit number of bytes to number available
    const size_t bytesAvailable = UsbCdcGetReadAvailable();
    if (numberOfBytes > bytesAvailable) {
        numberOfBytes = bytesAvailable;
    }

    // Read data
    readBufferReadIndex &= READ_WRITE_BUFFER_INDEX_BIT_MASK;
    CircularBufferRead(readBuffer, READ_WRITE_BUFFER_SIZE, &readBufferReadIndex, destination, numberOfBytes);
    return numberOfBytes;
}

/**
 * @brief Reads a byte from the read buffer.  This function must only be called
 * if there are bytes available in the read buffer.
 * @return Byte.
 */
uint8_t UsbCdcReadByte() {
    return readBuffer[readBufferReadIndex++ & READ_WRITE_BUFFER_INDEX_BIT_MASK];
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t UsbCdcGetWriteAvailable() {
    return (READ_WRITE_BUFFER_SIZE - 1) - ((writeBufferWriteIndex - writeBufferReadIndex) & READ_WRITE_BUFFER_INDEX_BIT_MASK);
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void UsbCdcWrite(const void* const data, const size_t numberOfBytes) {

    // Do nothing if no space avaliable
    if (UsbCdcGetWriteAvailable() < numberOfBytes) {
        return;
    }

    // Write data
    writeBufferWriteIndex &= READ_WRITE_BUFFER_INDEX_BIT_MASK;
    CircularBufferWrite(writeBuffer, READ_WRITE_BUFFER_SIZE, &writeBufferWriteIndex, data, numberOfBytes);
}

/**
 * @brief Writes a byte to the write buffer.
 * @param byte Byte.
 */
void UsbCdcWriteByte(const uint8_t byte) {

    // Do nothing if not enough space available
    if (UsbCdcGetWriteAvailable() == 0) {
        return;
    }

    // Write byte
    writeBuffer[writeBufferWriteIndex++ & READ_WRITE_BUFFER_INDEX_BIT_MASK] = byte;
}

//------------------------------------------------------------------------------
// End of file
