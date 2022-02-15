/**
 * @file DataLogger.c
 * @author Seb Madgwick
 * @brief Data logger.
 */

//------------------------------------------------------------------------------
// Includes

#include "CircularBuffer.h"
#include "DataLogger.h"
#include "definitions.h"
#include "SDCard/SDCard.h"
#include <stdbool.h>
#include <stdio.h> // printf, snprintf
#include <string.h> // strncpy, strlen
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief File name used while the file is open.
 */
#define FILE_PATH "/Data Logger/File"

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
// Function declarations

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

static DataLoggerSettings settings;
static DataLoggerCallbacks callbacks;
static State state = StateDisabled;
static uint64_t fileStartTicks;
static uint32_t fileSize;
static uint8_t bufferData[380000];
static CircularBuffer buffer = {.buffer = bufferData, .bufferSize = sizeof (bufferData)};
#ifdef PRINT_STATISTICS
static uint32_t maxWritePeriod;
static uint32_t maxbufferUsed;
static bool bufferOverrun;
#endif

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Sets the data logger settings.
 * @param settings_ Settings.
 */
void DataLoggerSetSettings(const DataLoggerSettings * const settings_) {

    // Store settings
    settings = *settings_;

    // Append underscore to file name prefix
    if (strlen(settings_->fileNamePrefix) > 0) {
        snprintf(settings.fileNamePrefix, sizeof (settings.fileNamePrefix), "%.*s_", (int) sizeof (settings.fileNamePrefix) - 2, settings_->fileNamePrefix);
    }

    // Prefix dot on file name extension
    if (strlen(settings_->fileExtension) > 0) {
        const char* fileExtension = settings_->fileExtension;
        while (*fileExtension == '.') {
            fileExtension++; // remove leading dots
        }
        snprintf(settings.fileExtension, sizeof (settings.fileExtension), ".%.*s", (int) sizeof (settings.fileExtension) - 2, fileExtension);
    }

    // Convert maximum file size to bytes
    if (settings.maxFileSize > (SIZE_MAX >> 20)) {
        settings.maxFileSize = 0;
    } else {
        settings.maxFileSize <<= 20;
    }

    // Convert maximum file period to ticks  
    if (settings.maxFilePeriod > (UINT64_MAX / TIMER_TICKS_PER_SECOND)) {
        settings.maxFilePeriod = 0;
    } else {
        settings.maxFilePeriod *= TIMER_TICKS_PER_SECOND;
    }
}

/**
 * @brief Sets the callback functions.
 * @param callbacks_ Callback functions.
 */
void DataLoggerSetCallbacks(const DataLoggerCallbacks * const callbacks_) {
    callbacks = *callbacks_;
}

/**
 * @brief Starts logging.
 */
void DataLoggerStart() {
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
    CircularBufferClear(&buffer);
    state = StateOpen;
}

/**
 * @brief Stops logging.
 */
void DataLoggerStop() {
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
 * @brief Returns the data logger status.
 * @return Data logger status.
 */
DataLoggerStatus DataLoggerGetSatus() {
    switch (state) {
        case StateDisabled:
            return DataLoggerStatusDisabled;
        case StateOpen:
        case StateWrite:
            return DataLoggerStatusEnabled;
        case StateError:
            return DataLoggerStatusError;
    }
    return DataLoggerStatusError;
}

/**
 * @brief Module tasks.  This function should be called repeatedly within the
 * main program loop.
 */
void DataLoggerTasks() {
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
    if (SDCardFileOpen(FILE_PATH, true) != SDCardErrorOK) {
#ifdef PRINT_STATISTICS
        printf("Open failed\r\n");
#endif
        return 1;
    }

    // Write preamble
    if (callbacks.writePreamble != NULL) {
        callbacks.writePreamble(); // use SDCardFileWrite to write preamble
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
    if (settings.maxFilePeriod > 0) {
        if (TimerGetTicks64() >= (fileStartTicks + settings.maxFilePeriod)) {
#ifdef PRINT_STATISTICS
            printf("Exceeded maximum file period\r\n");
#endif
            if ((CloseFile() != 0) || (OpenFile() != 0)) {
                return 1;
            }
            return 0;
        }
    }

    // Do nothing else if no data available
    const int writeIndex = buffer.writeIndex; // avoid asynchronous hazard
    if (buffer.readIndex == writeIndex) {
        return 0;
    }

    // Calculate number of bytes to write
    size_t numberOfBytes;
    int newReadIndex;
    if (writeIndex < buffer.readIndex) {
        numberOfBytes = buffer.bufferSize - buffer.readIndex;
        newReadIndex = 0;
    } else {
        numberOfBytes = writeIndex - buffer.readIndex;
        newReadIndex = writeIndex;
    }

    // Restart logging if maximum file size reached
    if (settings.maxFileSize > 0) {
        if (((uint64_t) fileSize + (uint64_t) numberOfBytes) >= (uint64_t) settings.maxFileSize) {
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
    const SDCardError sdCardError = SDCardFileWrite(&buffer.buffer[buffer.readIndex], numberOfBytes);
    buffer.readIndex = newReadIndex;
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
    char newFileName[SD_CARD_MAX_PATH_SIZE];
    if (settings.fileNameIsTime == false) {
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
    SDCardRename(FILE_PATH, SDCardPathJoin(2, SDCardPathSplitDirectory(FILE_PATH), newFileName));
    return 0;
}

/**
 * @brief Creates an available file name using an incrementing number.
 * @param destination Destination.
 * @param destinationSize Destination size.
 */
static void CreateFileNameUsingNumber(char* const destination, const size_t destinationSize) {

    // Open directory
    SDCardDirectoryOpen(SDCardPathSplitDirectory(FILE_PATH));

    // Create available file name
    const uint32_t initialFileNameNumber = settings.fileNameNumber;
    while (true) {
        snprintf(destination, destinationSize, "%s%04u%s", settings.fileNamePrefix, settings.fileNameNumber, settings.fileExtension);
        if (++settings.fileNameNumber > 9999) {
            settings.fileNameNumber = 0;
        }
        if (SDCardDirectoryExists(destination) == false) {
            if (callbacks.fileNameNumberChanged != NULL) {
                callbacks.fileNameNumberChanged(settings.fileNameNumber);
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
    SDCardDirectoryOpen(SDCardPathSplitDirectory(FILE_PATH));

    // Get file details
    SDCardFileDetails fileDetails;
    SDCardDirectorySearch(SDCardPathSplitFileName(FILE_PATH), &fileDetails);

    // Calculate file period
    const uint64_t filePeriod = (TimerGetTicks64() - fileStartTicks) / TIMER_TICKS_PER_SECOND;

    // Create available file name
    int counter = 0;
    while (true) {

        // Create counter string
        char counterString[16] = "";
        if (counter > 0) {
            snprintf(counterString, sizeof (counterString), "_%04u", counter);
        }

        // Create file name
        snprintf(destination, destinationSize, "%s%04u-%02u-%02u_%02u-%02u-%02u_%u%s%s",
                settings.fileNamePrefix,
                fileDetails.time.year,
                fileDetails.time.month,
                fileDetails.time.day,
                fileDetails.time.hour,
                fileDetails.time.minute,
                fileDetails.time.second,
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
size_t DataLoggerGetWriteAvailable() {
    return CircularBufferGetWriteAvailable(&buffer);
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void DataLoggerWrite(const void* const data, const size_t numberOfBytes) {

    // Do nothing if no space available
    if (numberOfBytes > DataLoggerGetWriteAvailable()) {
#ifdef PRINT_STATISTICS
        bufferOverrun = true;
#endif
        return;
    }

    // Write data
#ifdef PRINT_STATISTICS
    const uint32_t bufferUsed = sizeof (bufferData) - DataLoggerGetWriteAvailable();
    if (bufferUsed > maxbufferUsed) {
        maxbufferUsed = bufferUsed;
    }
#endif
    CircularBufferWrite(&buffer, data, numberOfBytes);
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
    snprintf(bufferUsageString, sizeof (bufferUsageString), "%0.1f %%", ((float) maxbufferUsed * (100.0f / (float) sizeof (bufferData))));

    // Print statistics
    printf("%u s, %u KB/s, %u KB, %0.1f ms, %s\r\n",
            (unsigned int) ((currentTicks - fileStartTicks) / TIMER_TICKS_PER_SECOND),
            (unsigned int) (kilobytesPerSecond + 0.5f),
            fileSize >> 10,
            (float) maxWritePeriod * (1000.0f / (float) TIMER_TICKS_PER_SECOND),
            bufferOverrun ? "Buffer Overrun" : bufferUsageString);

    // Reset statistics
    maxWritePeriod = 0;
    maxbufferUsed = 0;
    bufferOverrun = false;
}

#endif

//------------------------------------------------------------------------------
// End of file
