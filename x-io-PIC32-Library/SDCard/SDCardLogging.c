/**
 * @file SDCardLogging.c
 * @author Seb Madgwick
 * @brief Application interface for continuous logging of data to an SD card.
 */

//------------------------------------------------------------------------------
// Includes

#include "CircularBuffer.h"
#include "SDCard.h"
#include "SDCardLogging.h"
#include <stdbool.h>
#include <stdio.h> // printf, snprintf
#include <string.h> // strncpy, strlen
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Buffer size.
 */
#define WRITE_BUFFER_SIZE (400000)

/**
 * @brief File name used while the file is open.
 */
#define FILE_NAME "File"

/**
 * @brief Comment out this definition to disable printing of debug messages.
 */
#define DEBUG_ENABLED

/**
 * @brief State enumeration.
 */
typedef enum {
    StateDisabled,
    StateOpen,
    StateWrite,
    StateError,
} State;

//------------------------------------------------------------------------------
// Function prototypes

static void StateOpenTasks();
static void OpenFile();
static void StateWriteTasks();
static void CloseFile();
static void CreateFileNameUsingNumber(char* const destination, const size_t destinationSize);
static void CreateFileNameUsingTime(char* const destination, const size_t destinationSize);
static void PrintStatistics();

//------------------------------------------------------------------------------
// Variables

static SDCardLoggingSettings settings;
static SDCardLoggingCallbackFunctions callbackFunctions;
static State state = StateDisabled;
static uint64_t fileStartTicks;
static uint32_t fileSize;
static uint8_t __attribute__((persistent)) buffer[WRITE_BUFFER_SIZE];
static uint32_t bufferWriteIndex;
static uint32_t bufferReadIndex;
#ifdef DEBUG_ENABLED
static uint32_t maxWritePeriod;
static uint32_t maxbufferUsed;
static bool bufferOverrun;
#endif

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Sets the SD card logging settings.  This function must be called
 * before calling SDCardLoggingStart for the first time.
 * @param sdCardLoggingSettings SD card logging settings.
 */
void SDCardLoggingSetSettings(const SDCardLoggingSettings * const sdCardLoggingSettings) {

    // Copy structure
    settings = *sdCardLoggingSettings;

    // Append underscore to file name prefix
    if (strlen(sdCardLoggingSettings->fileNamePrefix) > 0) {
        snprintf(settings.fileNamePrefix, sizeof (settings.fileNamePrefix), "%.*s_", (int) sizeof (settings.fileNamePrefix) - 2, sdCardLoggingSettings->fileNamePrefix);
    }

    // Prefix dot on file name extension
    if (strlen(sdCardLoggingSettings->fileExtension) > 0) {
        const char* fileExtension = sdCardLoggingSettings->fileExtension;
        while (*fileExtension == '.') {
            fileExtension++; // remove leading dots
        }
        snprintf(settings.fileExtension, sizeof (settings.fileExtension), ".%.*s", (int) sizeof (settings.fileExtension) - 2, fileExtension);
    }
}

/**
 * @brief Sets the callback functions.
 * @param sdCardLoggingCallbackFunctions SD card logging callback functions.
 */
void SDCardLoggingSetCallbackFunctions(const SDCardLoggingCallbackFunctions * const sdCardLoggingCallbackFunctions) {
    callbackFunctions = *sdCardLoggingCallbackFunctions;
}

/**
 * @breif Starts logging.
 */
void SDCardLoggingStart() {
#ifdef DEBUG_ENABLED
    printf("Start\r\n");
#endif
    switch (state) {
        case StateDisabled:
            break;
        case StateOpen:
        case StateWrite:
            return;
        case StateError:
            break;
    }
    state = StateOpen;
}

/**
 * @breif Stops logging.
 */
void SDCardLoggingStop() {
#ifdef DEBUG_ENABLED
    printf("Stop\r\n");
#endif
    switch (state) {
        case StateDisabled:
            return;
        case StateOpen:
            break;
        case StateWrite:
            CloseFile();
            break;
        case StateError:
            return;
    }
    state = StateDisabled;
}

/**
 * @brief Returns the SD card logging status.
 * @return SD card logging status.
 */
SDCardLoggingStatus SDCardLoggingGetSatus() {
    switch (state) {
        case StateDisabled:
            return SDCardLoggingStatusDisabled;
        case StateOpen:
        case StateWrite:
            return SDCardLoggingStatusEnabled;
        case StateError:
            return SDCardLoggingStatusError;
    }
    return SDCardLoggingStatusError;
}

/**
 * @brief Module tasks.  This function should be called repeatedly within the
 * main program loop.
 */
void SDCardLoggingTasks() {
    switch (state) {
        case StateDisabled:
            break;
        case StateOpen:
            StateOpenTasks();
            break;
        case StateWrite:
            StateWriteTasks();
            break;
        case StateError:
            break;
    }
}

/**
 * @brief Open state tasks.
 */
static void StateOpenTasks() {

    // Do nothing if SD card not mounted
    if (SDCardIsMounted() == false) {
        return;
    }

    // Open file
    OpenFile();

    // Write preamble
    if (callbackFunctions.writePreamble != NULL) {
        callbackFunctions.writePreamble();
    }
    state = StateWrite;
}

/**
 * @brief Opens file.
 */
static void OpenFile() {
#ifdef DEBUG_ENABLED
    printf("Open\r\n");
#endif

    // Open file
    if (SDCardFileOpen(FILE_NAME, true) != SDCardErrorOK) {
#ifdef DEBUG_ENABLED
        printf("Open failed\r\n");
#endif
        state = StateError;
        return;
    }
    fileStartTicks = TimerGetTicks64();
    fileSize = 0;
#ifdef DEBUG_ENABLED
    maxWritePeriod = 0;
    maxbufferUsed = 0;
    bufferOverrun = false;
#endif
}

/**
 * @brief Write state tasks.
 */
static void StateWriteTasks() {

    // Print statistics
    PrintStatistics();

    // Restart logging if maximum file period reached
    if (settings.maximumFilePeriod > 0) {
        if (TimerGetTicks64() >= (fileStartTicks + settings.maximumFilePeriod)) {
#ifdef DEBUG_ENABLED
            printf("Exceeded maximum file period\r\n");
#endif
            CloseFile();
            OpenFile();
            return;
        }
    }

    // Do nothing if no data avaliable
    const uint32_t bufferWriteIndexCache = bufferWriteIndex; // avoid asynchronous hazard
    if (bufferReadIndex == bufferWriteIndexCache) {
        return;
    }

    // Calculate number of bytes to write
    size_t numberOfBytes;
    uint32_t newbufferReadIndex;
    if (bufferWriteIndexCache < bufferReadIndex) {
        numberOfBytes = WRITE_BUFFER_SIZE - bufferReadIndex;
        newbufferReadIndex = 0;
    } else {
        numberOfBytes = bufferWriteIndexCache - bufferReadIndex;
        newbufferReadIndex = bufferWriteIndexCache;
    }

    // Restart logging if maximum file size reached
    if (settings.maximumFileSize > 0) {
        if (((uint64_t) fileSize + (uint64_t) numberOfBytes) >= (uint64_t) settings.maximumFileSize) {
#ifdef DEBUG_ENABLED
            printf("Exceeded maximum file size\r\n");
#endif
            CloseFile();
            OpenFile();
            return;
        }
    }

    // Write data
#ifdef DEBUG_ENABLED
    const uint64_t writeStartTicks = TimerGetTicks64();
#endif
    const SDCardError sdCardError = SDCardFileWrite(&buffer[bufferReadIndex], numberOfBytes);
    bufferReadIndex = newbufferReadIndex;
    fileSize += numberOfBytes;
#ifdef DEBUG_ENABLED
    const uint64_t writePeriod = TimerGetTicks64() - writeStartTicks;
    if (writePeriod > maxWritePeriod) {
        maxWritePeriod = writePeriod;
    }
#endif

    // Restart logging if file full
    if (sdCardError == SDCardErrorFileOrSDCardFull) {
#ifdef DEBUG_ENABLED
        printf("SD card or file full\r\n");
#endif
        CloseFile();
        OpenFile();
        return;
    }

    // Abort if error occurred
    if (sdCardError != SDCardErrorOK) {
#ifdef DEBUG_ENABLED
        printf("Write failed\r\n");
#endif
        state = StateError;
        return;
    }
}

/**
 * @brief Close the file.
 */
static void CloseFile() {
#ifdef DEBUG_ENABLED
    printf("Close\r\n");
#endif

    // Close file
    SDCardFileClose();

    // Create new file name
    char newFileName[SD_CARD_MAX_FILE_NAME_SIZE];
    if (settings.fileNameIsTime == false) {
        CreateFileNameUsingNumber(newFileName, sizeof (newFileName));
    } else {
        CreateFileNameUsingTime(newFileName, sizeof (newFileName));
    }
    if (strlen(newFileName) == 0) {
#ifdef DEBUG_ENABLED
        printf("No file names available\r\n");
#endif
        state = StateError;
        return;
    }

    // Rename file
    SDCardFileRename(FILE_NAME, newFileName);
}

/**
 * @brief Creates an available file name using an incrementing number.
 * @param destination Destination.
 * @param destinationSize Destination size.
 */
static void CreateFileNameUsingNumber(char* const destination, const size_t destinationSize) {

    // Open directory
    SDCardDirectoryOpen();

    // Create available file name
    const uint32_t initialFileNameNumber = settings.fileNameNumber;
    while (true) {
        snprintf(destination, destinationSize, "%s%04u%s", settings.fileNamePrefix, settings.fileNameNumber, settings.fileExtension);
        if (++settings.fileNameNumber > 9999) {
            settings.fileNameNumber = 0;
        }
        if (SDCardDirectoryExists(destination) == false) {
            if (callbackFunctions.fileNameNumberChanged != NULL) {
                callbackFunctions.fileNameNumberChanged(settings.fileNameNumber);
            }
            break;
        }
        if (settings.fileNameNumber == initialFileNameNumber) {
            strncpy(destination, "", destinationSize); // no file names available
            break;
        }
    };

    // Close directory
    SDCardDirectoryClose();
}

/**
 * @brief Creates an available file name using the time.
 * @param destination Destination.
 * @param destinationSize Destination size.
 */
static void CreateFileNameUsingTime(char* const destination, const size_t destinationSize) {

    // Open directory
    SDCardDirectoryOpen();

    // Get file details
    SDCardFileDetails sdCardfileDetails;
    SDCardDirectorySearch(FILE_NAME, &sdCardfileDetails);

    // Calculate file period
    const uint64_t filePeriod = (TimerGetTicks64() - fileStartTicks) / TIMER_TICKS_PER_SECOND;

    // Create available file name
    uint32_t counter = 0;
    while (true) {

        // Create counter string
        char counterString[sizeof ("_0000")] = "";
        if (counter > 0) {
            snprintf(counterString, sizeof (counterString), "_%04u", counter);
        }

        // Create file name
        snprintf(destination, destinationSize, "%s%04u-%02u-%02u_%02u-%02u-%02u_%u%s%s",
                settings.fileNamePrefix,
                sdCardfileDetails.time.year,
                sdCardfileDetails.time.month,
                sdCardfileDetails.time.day,
                sdCardfileDetails.time.hour,
                sdCardfileDetails.time.minute,
                sdCardfileDetails.time.second,
                (unsigned int) filePeriod,
                counterString,
                settings.fileExtension);

        // Increment counter if file already exists
        if (SDCardDirectoryExists(destination) == false) {
            break;
        }
        if (++counter > 9999) {
            strncpy(destination, "", destinationSize); // no file names available
            break;
        }
    };

    // Close directory
    SDCardDirectoryClose();
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t SDCardLoggingGetWriteAvailable() {
    if (bufferWriteIndex < bufferReadIndex) {
        return (WRITE_BUFFER_SIZE - 1) - (WRITE_BUFFER_SIZE - bufferReadIndex) - bufferWriteIndex;
    } else {
        return (WRITE_BUFFER_SIZE - 1) - (bufferWriteIndex - bufferReadIndex);
    }
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void SDCardLoggingWrite(const void* const data, const size_t numberOfBytes) {

    // Do nothing if no space avaliable
    if (numberOfBytes > SDCardLoggingGetWriteAvailable()) {
#ifdef DEBUG_ENABLED
        bufferOverrun = true;
#endif
        return;
    }

    // Write data
#ifdef DEBUG_ENABLED
    const uint32_t bufferUsed = WRITE_BUFFER_SIZE - SDCardLoggingGetWriteAvailable();
    if (bufferUsed > maxbufferUsed) {
        maxbufferUsed = bufferUsed;
    }
#endif
    CircularBufferWrite(buffer, WRITE_BUFFER_SIZE, &bufferWriteIndex, data, numberOfBytes);
}

/**
 * @brief Prints statistics.
 */
static void PrintStatistics() {
#ifdef DEBUG_ENABLED

    // Reset values if new file
    static uint64_t previousTicks;
    static uint32_t previousFileSize;
    if (previousTicks < fileStartTicks) {
        previousTicks = fileStartTicks;
        previousFileSize = 0;
        return;
    }

    // Calculate delta ticks
    const uint64_t currentTicks = TimerGetTicks64();
    const uint64_t deltaTicks = currentTicks - previousTicks;

    // Do nothing else until period has elapsed
    if (deltaTicks < TIMER_TICKS_PER_SECOND) {
        return;
    }
    previousTicks = currentTicks;

    // Calculate bytes per second
    const uint32_t deltaFileSize = fileSize - previousFileSize;
    previousFileSize = fileSize;
    const float kilobytesPerSecond = (float) (deltaFileSize >> 10) / ((float) deltaTicks / (float) TIMER_TICKS_PER_SECOND);

    // Create buffer usage string
    char bufferUsageString[] = "Buffer Overrun";
    if (bufferOverrun == false) {
        const float percentage = (float) maxbufferUsed * (100.0f / (float) WRITE_BUFFER_SIZE);
        snprintf(bufferUsageString, sizeof (bufferUsageString), "%0.1f %%", (double) percentage);
    }

    // Create and print statistics string
    printf("%u s, %u KB/s, %u KB, %0.1f ms, %s\r\n",
            (unsigned int) ((currentTicks - fileStartTicks) / TIMER_TICKS_PER_SECOND),
            (unsigned int) (kilobytesPerSecond + 0.5f),
            fileSize >> 10,
            (double) ((float) maxWritePeriod * (1000.0f / (float) TIMER_TICKS_PER_SECOND)),
            bufferUsageString);

    // Reset statistics
    maxWritePeriod = 0;
    maxbufferUsed = 0;
    bufferOverrun = false;
#endif
}

//------------------------------------------------------------------------------
// End of file
