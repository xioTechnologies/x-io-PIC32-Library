/**
 * @file DataLogger.c
 * @author Seb Madgwick
 * @brief Data logger.
 */

//------------------------------------------------------------------------------
// Includes

#include "DataLogger.h"
#include "definitions.h"
#include "Fifo.h"
#include "Rtc/Rtc.h"
#include "SDCard/SDCard.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Counter file name.
 */
#define COUNTER_FILE_NAME "Counter.txt"

/**
 * @brief Uncomment this line to enable printing of statistics.
 */
//#define PRINT_STATISTICS

/**
 * @brief State.
 */
typedef enum {
    StateDisabled,
    StateOpen,
    StateWrite,
} State;

//------------------------------------------------------------------------------
// Function declarations

static void StateOpenTasks(void);
static void StateWriteTasks(void);
static int Open(void);
static unsigned int ReadCounter(void);
static void WriteCounter(const unsigned int counter);
static int Write(void);
static void Close(void);
static void StatusCallback(const DataLoggerStatus status);
static void ErrorCallback(const DataLoggerError error);
#ifdef PRINT_STATISTICS
static void PrintStatistics(void);
#endif

//------------------------------------------------------------------------------
// Variables

static DataLoggerSettings settings;
static DataLoggerCallbacks callbacks;
static State state = StateDisabled;
char fileName[SD_CARD_MAX_PATH_SIZE];
static uint64_t fileStartTicks;
static uint32_t fileSize;
static uint8_t fifoData[380000];
static Fifo fifo = {.data = fifoData, .dataSize = sizeof (fifoData)};
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
 * @brief Sets the callbacks.
 * @param callbacks_ Callbacks.
 */
void DataLoggerSetCallbacks(const DataLoggerCallbacks * const callbacks_) {
    callbacks = *callbacks_;
}

/**
 * @brief Module tasks. This function should be called repeatedly within the
 * main program loop.
 */
void DataLoggerTasks(void) {
    switch (state) {
        case StateDisabled:
            break;
        case StateOpen:
            StateOpenTasks();
            break;
        case StateWrite:
            StateWriteTasks();
            break;
    }
}

/**
 * @brief Open state tasks.
 */
static void StateOpenTasks(void) {

    // Do nothing if SD card not mounted
    if (SDCardMounted() == false) {
        return;
    }

    // Open file
    if (Open() != 0) {
        state = StateDisabled;
        return;
    }
    state = StateWrite;
}

/**
 * @brief Write state tasks.
 */
static void StateWriteTasks(void) {
#ifdef PRINT_STATISTICS
    PrintStatistics();
#endif
    if (Write() != 0) {
        state = StateDisabled;
    }
}

/**
 * @brief Opens the file.
 * @return 0 if successful.
 */
static int Open(void) {

    // Open directory
    SDCardDirectoryOpen(settings.directory);

    // Create file name
    bool fileNameUnavailable = false;
    switch (settings.fileNameSuffix) {
        case DataLoggerSuffixNone:
            snprintf(fileName, sizeof (fileName), "%s%s", settings.fileNamePrefix, settings.fileExtension);
            fileNameUnavailable = SDCardDirectoryExists(fileName);
            break;
        case DataLoggerSuffixCounter:
        {
            const uint32_t initialCounter = ReadCounter();
            uint32_t counter = initialCounter;
            while (true) {
                snprintf(fileName, sizeof (fileName), "%s%s%04u%s", settings.fileNamePrefix, strlen(settings.fileNamePrefix) > 0 ? "_" : "", counter, settings.fileExtension);
                if (++counter > 9999) {
                    counter = 0;
                }
                if (SDCardDirectoryExists(fileName) == false) {
                    break;
                }
                if (counter == initialCounter) {
                    fileNameUnavailable = true;
                    break;
                }
            }
            WriteCounter(counter);
            break;
        }
        case DataLoggerSuffixTime:
        {
            RtcTime time;
            RtcGetTime(&time);
            snprintf(fileName, sizeof (fileName), "%s%s%04u-%02u-%02u_%02u-%02u-%02u%s",
                    settings.fileNamePrefix,
                    strlen(settings.fileNamePrefix) > 0 ? "_" : "",
                    time.year, time.month, time.day, time.hour, time.minute, time.second,
                    settings.fileExtension);
            fileNameUnavailable = SDCardDirectoryExists(fileName);
            break;
        }
    }

    // Close directory
    SDCardDirectoryClose();

    // Abort if file name unavailable
    if (fileNameUnavailable) {
        ErrorCallback(DataLoggerErrorFileNameUnavailable);
        return 1;
    }

    // Open file
    switch (SDCardFileOpen(SDCardPathJoin(2, settings.directory, fileName), true)) {
        case SDCardErrorOK:
            break;
        case SDCardErrorFileSystemError:
            ErrorCallback(DataLoggerErrorFileSystemError);
            return 1;
        case SDCardErrorFileOrSDCardFull:
            ErrorCallback(DataLoggerErrorSDCardFull);
            return 1;
    }
    StatusCallback(DataLoggerStatusOpen);
#ifdef PRINT_STATISTICS
    printf("%s\n", fileName);
#endif

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
 * @brief Reads the counter from file.
 * @return Counter.
 */
static unsigned int ReadCounter(void) {
    if (SDCardFileOpen(SDCardPathJoin(2, settings.directory, COUNTER_FILE_NAME), false) != SDCardErrorOK) {
        return 0;
    }
    char string[8];
    const size_t numberOfBytes = SDCardFileRead(string, sizeof (string));
    SDCardFileClose();
    if (numberOfBytes == -1) {
        return 0;
    }
    unsigned int counter;
    if (sscanf(string, "%u", &counter) != 1) {
        return 0;
    }
    return counter;
}

/**
 * @brief Writes the counter to file.
 * @param Counter.
 */
static void WriteCounter(const unsigned int counter) {
    if (SDCardFileOpen(SDCardPathJoin(2, settings.directory, COUNTER_FILE_NAME), true) != SDCardErrorOK) {
        return;
    }
    char string[8];
    snprintf(string, sizeof (string), "%04u", counter);
    SDCardFileWriteString(string);
    SDCardFileClose();
}

/**
 * @brief Writes buffered data to the file.
 * @return 0 if successful.
 */
static int Write(void) {

    // Restart logging if maximum file period reached
    if (settings.maxFilePeriod > 0) {
        if (TimerGetTicks64() >= (fileStartTicks + settings.maxFilePeriod)) {
            StatusCallback(DataLoggerStatusMaxFilePeriodExceeded);
            Close();
            return Open();
        }
    }

    // Do nothing else if no data available
    const size_t writeIndex = fifo.writeIndex; // avoid asynchronous hazard
    if (fifo.readIndex == writeIndex) {
        return 0;
    }

    // Calculate number of bytes to write
    size_t numberOfBytes;
    size_t newReadIndex;
    if (writeIndex < fifo.readIndex) {
        numberOfBytes = fifo.dataSize - fifo.readIndex;
        newReadIndex = 0;
    } else {
        numberOfBytes = writeIndex - fifo.readIndex;
        newReadIndex = writeIndex;
    }

    // Restart logging if maximum file size reached
    if (settings.maxFileSize > 0) {
        if (((uint64_t) fileSize + (uint64_t) numberOfBytes) >= (uint64_t) settings.maxFileSize) {
            StatusCallback(DataLoggerStatusMaxFileSizeExceeded);
            Close();
            return Open();
        }
    }

    // Write data
#ifdef PRINT_STATISTICS
    const uint64_t writeStart = TimerGetTicks64();
#endif
    const SDCardError error = SDCardFileWrite(&fifo.data[fifo.readIndex], numberOfBytes);
    fifo.readIndex = newReadIndex;
    fileSize += numberOfBytes;
#ifdef PRINT_STATISTICS
    const uint64_t writePeriod = TimerGetTicks64() - writeStart;
    if (writePeriod > maxWritePeriod) {
        maxWritePeriod = writePeriod;
    }
#endif

    // Restart logging if file full
    if (error == SDCardErrorFileOrSDCardFull) {
        StatusCallback(DataLoggerStatusSDCardOrFileFull);
        Close();
        return Open();
    }

    // Abort if error occurred
    if (error != SDCardErrorOK) {
        ErrorCallback(DataLoggerErrorFileSystemError);
        return 1;
    }
    return 0;
}

/**
 * @brief Closes the file.
 */
static void Close(void) {
    StatusCallback(DataLoggerStatusClose);
    SDCardFileClose();
}

/**
 * @brief Starts logging.
 */
void DataLoggerStart(void) {
    StatusCallback(DataLoggerStatusStart);
    switch (state) {
        case StateDisabled:
            break;
        case StateOpen:
        case StateWrite:
            return;
    }
    FifoClear(&fifo);
    state = StateOpen;
}

/**
 * @brief Stops logging.
 */
void DataLoggerStop(void) {
    StatusCallback(DataLoggerStatusStop);
    switch (state) {
        case StateDisabled:
            break;
        case StateOpen:
            state = StateDisabled;
            break;
        case StateWrite:
            state = StateDisabled;
            if ((Write() != 0) || (Write() != 0)) { // write twice to handle buffer index wraparound
                break;
            }
            Close();
            break;
    }
}

/**
 * @brief Returns true if logging is enabled.
 * @return True if logging is enabled.
 */
bool DataLoggerEnabled(void) {
    switch (state) {
        case StateDisabled:
            return false;
        case StateOpen:
        case StateWrite:
            break;
    }
    return true;
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t DataLoggerGetWriteAvailable(void) {
    return FifoGetWriteAvailable(&fifo);
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
    const uint32_t bufferUsed = sizeof (fifoData) - DataLoggerGetWriteAvailable();
    if (bufferUsed > maxbufferUsed) {
        maxbufferUsed = bufferUsed;
    }
#endif
    FifoWrite(&fifo, data, numberOfBytes);
}

/**
 * @brief Returns the file name of the current file.
 * @return File name of the current file.
 */
const char* DataLoggerGetFileName(void) {
    return fileName;
}

/**
 * @brief Calls the status callback.
 * @param status Status.
 */
static void StatusCallback(const DataLoggerStatus status) {
#ifdef PRINT_STATISTICS
    printf("%s\n", DataLoggerStatusToString(status));
#endif
    if (callbacks.status != NULL) {
        callbacks.status(status);
    }
}

/**
 * @brief Calls the error callback.
 * @param error Error.
 */
static void ErrorCallback(const DataLoggerError error) {
#ifdef PRINT_STATISTICS
    printf("%s\n", DataLoggerErrorToString(error));
#endif
    if (callbacks.error != NULL) {
        callbacks.error(error);
    }
}

/**
 * @brief Returns the status message.
 * @param status Status
 * @return Status message.
 */
const char* DataLoggerStatusToString(const DataLoggerStatus status) {
    switch (status) {
        case DataLoggerStatusStart:
            return "Start";
        case DataLoggerStatusStop:
            return "Stop";
        case DataLoggerStatusOpen:
            return "Open";
        case DataLoggerStatusMaxFileSizeExceeded:
            return "Max file size exceeded";
        case DataLoggerStatusMaxFilePeriodExceeded:
            return "Max file period exceeded";
        case DataLoggerStatusSDCardOrFileFull:
            return "SD card or file full";
        case DataLoggerStatusClose:
            return "Close";
    }
    return ""; // avoid compiler warning
}

/**
 * @brief Returns the error message.
 * @param error Error
 * @return Error message.
 */
const char* DataLoggerErrorToString(const DataLoggerError error) {
    switch (error) {
        case DataLoggerErrorFileNameUnavailable:
            return "File name unavailable";
        case DataLoggerErrorSDCardFull:
            return "SD card full";
        case DataLoggerErrorFileSystemError:
            return "File system error";
    }
    return ""; // avoid compiler warning
}

#ifdef PRINT_STATISTICS

/**
 * @brief Prints statistics.
 */
static void PrintStatistics(void) {

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
    snprintf(bufferUsageString, sizeof (bufferUsageString), "%0.1f%%", ((float) maxbufferUsed * (100.0f / (float) sizeof (fifoData))));

    // Print statistics
    printf("%u s, %u KB, %u KB/s, %0.1f ms, %s\n",
            (unsigned int) ((currentTicks - fileStartTicks) / TIMER_TICKS_PER_SECOND),
            fileSize >> 10,
            (unsigned int) (kilobytesPerSecond + 0.5f),
            (float) maxWritePeriod * (1.0f / (float) TIMER_TICKS_PER_MILLISECOND),
            bufferOverrun ? "Buffer Overrun" : bufferUsageString);

    // Reset statistics
    maxWritePeriod = 0;
    maxbufferUsed = 0;
    bufferOverrun = false;
}

#endif

//------------------------------------------------------------------------------
// End of file
