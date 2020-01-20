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
#define BUFFER_SIZE (400000)

/**
 * @brief File name used while the file is open.
 */
#define FILE_NAME "File"

/**
 * @brief Comment out this definition to disable printing of statistics.
 */
//#define PRINT_STATISTICS

/**
 * @brief State.
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
static void StateWriteTasks();
static int OpenFile();
static int WriteToFile();
static int CloseFile();
static void CreateFileNameUsingNumber(char* const destination, const size_t destinationSize);
static void CreateFileNameUsingTime(char* const destination, const size_t destinationSize);
#ifdef PRINT_STATISTICS
static void PrintStatistics();
#endif

//------------------------------------------------------------------------------
// Variables

static SDCardLoggingSettings currentSettings;
static SDCardLoggingCallbacks applicationCallbacks;
static State state = StateDisabled;
static uint64_t fileStartTicks;
static uint32_t fileSize;
static uint8_t __attribute__((persistent)) buffer[BUFFER_SIZE];
static int bufferWriteIndex;
static int bufferReadIndex;
#ifdef PRINT_STATISTICS
static uint32_t maxWritePeriod;
static uint32_t maxbufferUsed;
static bool bufferOverrun;
#endif

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Sets the SD card logging settings.
 * @param settings Settings.
 */
void SDCardLoggingSetSettings(const SDCardLoggingSettings * const settings) {

    // Copy structure
    currentSettings = *settings;

    // Append underscore to file name prefix
    if (strlen(settings->fileNamePrefix) > 0) {
        snprintf(currentSettings.fileNamePrefix, sizeof (currentSettings.fileNamePrefix), "%.*s_", (int) sizeof (currentSettings.fileNamePrefix) - 2, settings->fileNamePrefix);
    }

    // Prefix dot on file name extension
    if (strlen(settings->fileExtension) > 0) {
        const char* fileExtension = settings->fileExtension;
        while (*fileExtension == '.') {
            fileExtension++; // remove leading dots
        }
        snprintf(currentSettings.fileExtension, sizeof (currentSettings.fileExtension), ".%.*s", (int) sizeof (currentSettings.fileExtension) - 2, fileExtension);
    }
}

/**
 * @brief Sets the callback functions.
 * @param callbacks Callback functions.
 */
void SDCardLoggingSetCallbacks(const SDCardLoggingCallbacks * const callbacks) {
    applicationCallbacks = *callbacks;
}

/**
 * @brief Starts logging.
 */
void SDCardLoggingStart() {
#ifdef PRINT_STATISTICS
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
    bufferWriteIndex = bufferReadIndex; // clear buffer
    state = StateOpen;
}

/**
 * @brief Stops logging.
 */
void SDCardLoggingStop() {
#ifdef PRINT_STATISTICS
    printf("Stop\r\n");
#endif
    switch (state) {
        case StateDisabled:
            break;
        case StateOpen:
            state = StateDisabled;
            break;
        case StateWrite:
            state = StateDisabled;
            if ((WriteToFile() != 0) || (WriteToFile() != 0) || (CloseFile() != 0)) { // write twice to handle buffer index wraparound
                state = StateError;
            }
            break;
        case StateError:
            break;
    }
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
    if (OpenFile() != 0) {
        state = StateError;
        return;
    }
    state = StateWrite;
}

/**
 * @brief Write state tasks.
 */
static void StateWriteTasks() {
#ifdef PRINT_STATISTICS
    PrintStatistics();
#endif
    if (WriteToFile() != 0) {
        state = StateError;
    }
}

/**
 * @brief Opens a file.
 * @return 0 if successful.
 */
static int OpenFile() {
#ifdef PRINT_STATISTICS
    printf("Open\r\n");
#endif

    // Open file
    if (SDCardFileOpen(FILE_NAME, true) != SDCardErrorOK) {
#ifdef PRINT_STATISTICS
        printf("Open failed\r\n");
#endif
        return 1;
    }

    // Write preamble
    if (applicationCallbacks.writePreamble != NULL) {
        applicationCallbacks.writePreamble(); // use SDCardFileWrite to write preamble
    }

    // Reset statistics
    fileStartTicks = TimerGetTicks64();
    fileSize = SDCardFileGetSize();
#ifdef PRINT_STATISTICS
    maxWritePeriod = 0;
    maxbufferUsed = 0;
    bufferOverrun = false;
#endif
    return 0;
}

/**
 * @brief Writes buffered data to the file.
 * @return 0 if successful.
 */
static int WriteToFile() {

    // Restart logging if maximum file period reached
    if (currentSettings.maximumFilePeriod > 0) {
        if (TimerGetTicks64() >= (fileStartTicks + currentSettings.maximumFilePeriod)) {
#ifdef PRINT_STATISTICS
            printf("Exceeded maximum file period\r\n");
#endif
            if ((CloseFile() != 0) || (OpenFile() != 0)) {
                return 1;
            }
            return 0;
        }
    }

    // Do nothing else if no data avaliable
    const int bufferWriteIndexCache = bufferWriteIndex; // avoid asynchronous hazard
    if (bufferReadIndex == bufferWriteIndexCache) {
        return 0;
    }

    // Calculate number of bytes to write
    size_t numberOfBytes;
    int newbufferReadIndex;
    if (bufferWriteIndexCache < bufferReadIndex) {
        numberOfBytes = BUFFER_SIZE - bufferReadIndex;
        newbufferReadIndex = 0;
    } else {
        numberOfBytes = bufferWriteIndexCache - bufferReadIndex;
        newbufferReadIndex = bufferWriteIndexCache;
    }

    // Restart logging if maximum file size reached
    if (currentSettings.maximumFileSize > 0) {
        if (((uint64_t) fileSize + (uint64_t) numberOfBytes) >= (uint64_t) currentSettings.maximumFileSize) {
#ifdef PRINT_STATISTICS
            printf("Exceeded maximum file size\r\n");
#endif
            if ((CloseFile() != 0) || (OpenFile() != 0)) {
                return 1;
            }
            return 0;
        }
    }

    // Write data
#ifdef PRINT_STATISTICS
    const uint64_t writeStartTicks = TimerGetTicks64();
#endif
    const SDCardError sdCardError = SDCardFileWrite(&buffer[bufferReadIndex], numberOfBytes);
    bufferReadIndex = newbufferReadIndex;
    fileSize += numberOfBytes;
#ifdef PRINT_STATISTICS
    const uint64_t writePeriod = TimerGetTicks64() - writeStartTicks;
    if (writePeriod > maxWritePeriod) {
        maxWritePeriod = writePeriod;
    }
#endif

    // Restart logging if file full
    if (sdCardError == SDCardErrorFileOrSDCardFull) {
#ifdef PRINT_STATISTICS
        printf("SD card or file full\r\n");
#endif
        if ((CloseFile() != 0) || (OpenFile() != 0)) {
            return 1;
        }
        return 0;
    }

    // Abort if error occurred
    if (sdCardError != SDCardErrorOK) {
#ifdef PRINT_STATISTICS
        printf("Write failed\r\n");
#endif
        return 1;
    }
    return 0;
}

/**
 * @brief Closes the file.
 * @return 0 if successful.
 */
static int CloseFile() {
#ifdef PRINT_STATISTICS
    printf("Close\r\n");
#endif

    // Close file
    SDCardFileClose();

    // Create new file name
    char newFileName[SD_CARD_MAX_FILE_NAME_SIZE];
    if (currentSettings.fileNameIsTime == false) {
        CreateFileNameUsingNumber(newFileName, sizeof (newFileName));
    } else {
        CreateFileNameUsingTime(newFileName, sizeof (newFileName));
    }
    if (strlen(newFileName) == 0) {
#ifdef PRINT_STATISTICS
        printf("No file names available\r\n");
#endif
        return 1;
    }

    // Rename file
    SDCardFileRename(FILE_NAME, newFileName);
    return 0;
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
    const uint32_t initialFileNameNumber = currentSettings.fileNameNumber;
    while (true) {
        snprintf(destination, destinationSize, "%s%04u%s", currentSettings.fileNamePrefix, currentSettings.fileNameNumber, currentSettings.fileExtension);
        if (++currentSettings.fileNameNumber > 9999) {
            currentSettings.fileNameNumber = 0;
        }
        if (SDCardDirectoryExists(destination) == false) {
            if (applicationCallbacks.fileNameNumberChanged != NULL) {
                applicationCallbacks.fileNameNumberChanged(currentSettings.fileNameNumber);
            }
            break;
        }
        if (currentSettings.fileNameNumber == initialFileNameNumber) {
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
    int counter = 0;
    while (true) {

        // Create counter string
        char counterString[sizeof ("_0000")] = "";
        if (counter > 0) {
            snprintf(counterString, sizeof (counterString), "_%04u", counter);
        }

        // Create file name
        snprintf(destination, destinationSize, "%s%04u-%02u-%02u_%02u-%02u-%02u_%u%s%s",
                currentSettings.fileNamePrefix,
                sdCardfileDetails.time.year,
                sdCardfileDetails.time.month,
                sdCardfileDetails.time.day,
                sdCardfileDetails.time.hour,
                sdCardfileDetails.time.minute,
                sdCardfileDetails.time.second,
                (unsigned int) filePeriod,
                counterString,
                currentSettings.fileExtension);

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
        return (BUFFER_SIZE - 1) - (BUFFER_SIZE - bufferReadIndex) - bufferWriteIndex;
    } else {
        return (BUFFER_SIZE - 1) - (bufferWriteIndex - bufferReadIndex);
    }
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void SDCardLoggingWrite(const void* const data, const size_t numberOfBytes) {

    // Do nothing if logging not started
    switch (state) {
        case StateDisabled:
            return;
        case StateOpen:
        case StateWrite:
            break;
        case StateError:
            return;
    }

    // Do nothing if no space avaliable
    if (numberOfBytes > SDCardLoggingGetWriteAvailable()) {
#ifdef PRINT_STATISTICS
        bufferOverrun = true;
#endif
        return;
    }

    // Write data
#ifdef PRINT_STATISTICS
    const uint32_t bufferUsed = BUFFER_SIZE - SDCardLoggingGetWriteAvailable();
    if (bufferUsed > maxbufferUsed) {
        maxbufferUsed = bufferUsed;
    }
#endif
    CircularBufferWrite(buffer, BUFFER_SIZE, &bufferWriteIndex, data, numberOfBytes);
}

#ifdef PRINT_STATISTICS

/**
 * @brief Prints statistics.
 */
static void PrintStatistics() {

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
    char bufferUsageString[16];
    snprintf(bufferUsageString, sizeof (bufferUsageString), "%0.1f %%", (double) ((float) maxbufferUsed * (100.0f / (float) BUFFER_SIZE)));

    // Create and print statistics string
    printf("%u s, %u KB/s, %u KB, %0.1f ms, %s\r\n",
            (unsigned int) ((currentTicks - fileStartTicks) / TIMER_TICKS_PER_SECOND),
            (unsigned int) (kilobytesPerSecond + 0.5f),
            fileSize >> 10,
            (double) ((float) maxWritePeriod * (1000.0f / (float) TIMER_TICKS_PER_SECOND)),
            bufferOverrun ? "Buffer Overrun" : bufferUsageString);

    // Reset statistics
    maxWritePeriod = 0;
    maxbufferUsed = 0;
    bufferOverrun = false;
}

#endif

//------------------------------------------------------------------------------
// End of file
