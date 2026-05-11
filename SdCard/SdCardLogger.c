/**
 * @file SdCardLogger.c
 * @author Seb Madgwick
 * @brief SD card logger.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "definitions.h"
#include "Fifo.h"
#include "Periodic.h"
#include "RtcWeak/RtcWeak.h"
#include "SdCard/SdCard.h"
#include "SdCardLogger.h"
#include <stdio.h>
#include <string.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Uncomment this line to print statistics.
 */
//#define PRINT_STATISTICS

/**
 * @brief Counter file path.
 */
#define COUNTER_FILE_PATH SD_CARD_LOGGER_DIRECTORY "/Counter.txt"

/**
 * @brief Counter maximum value.
 */
#define COUNTER_MAX_VALUE (99999)

/**
 * @brief State.
 */
typedef enum {
    StateDisabled,
    StateOpen,
    StateWriting,
} State;

/**
 * @brief Result.
 */
typedef enum {
    ResultOk,
    ResultError,
} Result;

//------------------------------------------------------------------------------
// Function declarations

static Result Open(void);
static int ReadCounter(void);
static void WriteCounter(const int counter);
static Result Write(void);
static Result Close(void);
static Result CloseThenOpen(void);
static void EventCallback(const SdCardLoggerEvent event);
static void ErrorCallback(const SdCardLoggerError error);

//------------------------------------------------------------------------------
// Variables

static SdCardLoggerSettings settings;
static SdCardLoggerCallbacks callbacks;
static State state = StateDisabled;
static char fileName[SD_CARD_MAX_PATH_SIZE];
static size_t fileSize;
static uint64_t fileTimeout;
static uint8_t fifoData[SD_CARD_LOGGER_BUFFER_SIZE];
static Fifo fifo = {.data = fifoData, .dataSize = sizeof (fifoData)};
#ifdef PRINT_STATISTICS
static uint64_t startTime;
static uint64_t previousTime;
static size_t previousSize;
static uint64_t maxBlockingDuration;
static size_t maxBufferUsed;
#endif

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Sets the settings.
 * @param settings_ Settings.
 */
void SdCardLoggerSetSettings(const SdCardLoggerSettings * const settings_) {
    settings = *settings_;
    if ((settings.maxFileSize == 0) || (settings.maxFileSize > (size_t) SD_CARD_MAX_FILE_SIZE)) {
        settings.maxFileSize = (size_t) SD_CARD_MAX_FILE_SIZE;
    }
}

/**
 * @brief Sets the callbacks.
 * @param callbacks_ Callbacks.
 */
void SdCardLoggerSetCallbacks(const SdCardLoggerCallbacks * const callbacks_) {
    callbacks = *callbacks_;
}

/**
 * @brief Module tasks. This function should be called repeatedly within the
 * main program loop.
 */
void SdCardLoggerTasks(void) {

    // Module tasks
#ifdef PRINT_STATISTICS
    const uint64_t blockingStart = TimerGetTicks64();
#endif
    switch (state) {
        case StateDisabled:
            return;
        case StateOpen:
            if (Open() != ResultOk) {
                state = StateDisabled;
            } else {
                state = StateWriting;
            }
            break;
        case StateWriting:
            if (Write() != ResultOk) {
                state = StateDisabled;
            }
            break;
    }

    // Update max blocking duration
#ifdef PRINT_STATISTICS
    const uint64_t blockingDuration = TimerGetTicks64() - blockingStart;
    if (blockingDuration > maxBlockingDuration) {
        maxBlockingDuration = blockingDuration;
    }

    // Calculate delta time
    const uint64_t now = TimerGetTicks64();
    const uint64_t deltaTime = now - previousTime;

    // Do nothing else until period has elapsed
    if (deltaTime < TIMER_TICKS_PER_SECOND) {
        return;
    }

    // Print
    printf("%u s, %zu KB, %zu KB/s, %0.1f ms, %0.1f%%\n",
            (unsigned int) ((now - startTime) / TIMER_TICKS_PER_SECOND),
            fileSize >> 10,
            (size_t) ((float) ((fileSize - previousSize) >> 10) / ((float) deltaTime / (float) TIMER_TICKS_PER_SECOND)),
            (float) maxBlockingDuration * (1.0f / (float) TIMER_TICKS_PER_MILLISECOND),
            (float) maxBufferUsed * (100.0f / (float) FifoCapacity(&fifo)));

    // Reset values
    previousTime = now;
    previousSize = fileSize;
    maxBlockingDuration = 0;
    maxBufferUsed = 0;
#endif
}

/**
 * @brief Opens a new file.
 * @return Result.
 */
static Result Open(void) {

    // Open directory
    if (SdCardDirectoryOpen(SD_CARD_LOGGER_DIRECTORY) != SdCardResultOk) {
        return ResultError;
    }

    // Create file name
    bool fileAlreadyExists = false;
    switch (settings.fileNameSuffix) {
        case SdCardLoggerSuffixNone:
            snprintf(fileName, sizeof (fileName), "%s%s", settings.fileNamePrefix, settings.fileExtension);
            fileAlreadyExists = SdCardDirectoryExists(fileName);
            break;
        case SdCardLoggerSuffixCounter:
        {
            const int initialCounter = ReadCounter();
            int counter = initialCounter;
            while (true) {
                snprintf(fileName, sizeof (fileName), "%s%s%05d%s",
                        settings.fileNamePrefix,
                        strlen(settings.fileNamePrefix) > 0 ? "_" : "",
                        counter,
                        settings.fileExtension);
                if (++counter > COUNTER_MAX_VALUE) {
                    counter = 0;
                }
                if (SdCardDirectoryExists(fileName) == false) {
                    break;
                }
                if (counter == initialCounter) {
                    fileAlreadyExists = true;
                    break;
                }
            }
            if (fileAlreadyExists == false) {
                WriteCounter(counter);
            }
            break;
        }
        case SdCardLoggerSuffixDateAndTime:
        {
            RtcTime time;
            RtcGetTime(&time);
            snprintf(fileName, sizeof (fileName), "%s%s%04u-%02u-%02u_%02u-%02u-%02u%s",
                    settings.fileNamePrefix,
                    strlen(settings.fileNamePrefix) > 0 ? "_" : "",
                    time.year, time.month, time.day, time.hour, time.minute, time.second,
                    settings.fileExtension);
            fileAlreadyExists = SdCardDirectoryExists(fileName);
            break;
        }
    }

    // Close directory
    if (SdCardDirectoryClose() != SdCardResultOk) {
        return ResultError;
    }

    // Abort if file name unavailable
    if (fileAlreadyExists) {
        ErrorCallback(SdCardLoggerErrorFileAlreadyExists);
        return ResultError;
    }

    // Create file
    char filePath[SD_CARD_MAX_PATH_SIZE];
    SdCardPathJoin(filePath, sizeof (filePath), 2, SD_CARD_LOGGER_DIRECTORY, fileName);
    if (SdCardFileCreate(filePath) != SdCardResultOk) {
        return ResultError;
    }
    EventCallback(SdCardLoggerEventOpen); // may be used to write preamble
    if (SdCardFileGetSize(&fileSize) != SdCardResultOk) {
        SdCardFileClose();
        return ResultError;
    }
    fileTimeout = TimerGetTicks64() + ((uint64_t) settings.maxFilePeriod * TIMER_TICKS_PER_SECOND);

    // Reset statistics
#ifdef PRINT_STATISTICS
    printf("%s\n", fileName);
    startTime = TimerGetTicks64();
    previousTime = 0;
    previousSize = 0;
    maxBlockingDuration = 0;
    maxBufferUsed = 0;
#endif
    return ResultOk;
}

/**
 * @brief Reads the counter from file.
 * @return Counter.
 */
static int ReadCounter(void) {
    char string[16];
    if (SdCardFileQuickReadString(COUNTER_FILE_PATH, string, sizeof (string)) != SdCardResultOk) {
        return 0;
    }
    int counter;
    if (sscanf(string, "%d", &counter) != 1) {
        return 0;
    }
    if ((counter < 0) || (counter > COUNTER_MAX_VALUE)) {
        return 0;
    }
    return counter;
}

/**
 * @brief Writes the counter to file.
 * @param counter Counter.
 */
static void WriteCounter(const int counter) {
    char string[16];
    snprintf(string, sizeof (string), "%05d", counter);
    SdCardFileQuickWriteString(COUNTER_FILE_PATH, string);
}

/**
 * @brief Writes data to the file.
 * @return Result.
 */
static Result Write(void) {

    // Update max buffer used
#ifdef PRINT_STATISTICS
    const size_t bufferUsed = FifoAvailableRead(&fifo);
    if (bufferUsed > maxBufferUsed) {
        maxBufferUsed = bufferUsed;
    }
#endif

    // Open new file if maximum file period exceeded
    if (settings.maxFilePeriod != 0) {
        if (TimerGetTicks64() >= fileTimeout) {
            EventCallback(SdCardLoggerEventMaxFilePeriodExceeded);
            return CloseThenOpen();
        }
    }

    // Flush file
    if (PERIODIC_POLL(1.0f)) {
        if (SdCardFileFlush() != SdCardResultOk) {
            return ResultError;
        }
    }

    // Do nothing else if no data available
    size_t numberOfBytes;
    volatile void* data;
    FifoReadPointer(&fifo, &data, &numberOfBytes);
    if (numberOfBytes == 0) {
        return ResultOk;
    }

    // Open new file if maximum file size exceeded
    if (((uint64_t) fileSize + (uint64_t) numberOfBytes) >= (uint64_t) settings.maxFileSize) {
        EventCallback(SdCardLoggerEventMaxFileSizeExceeded);
        return CloseThenOpen();
    }

    // Write data
    const SdCardResult result = SdCardFileWrite((void*) data, numberOfBytes);
    FifoReadPointerComplete(&fifo, numberOfBytes);
    fileSize += numberOfBytes;

    // Abort if SD card full
    if (result == SdCardResultFileOrSdCardFull) {
        ErrorCallback(SdCardLoggerErrorSdCardFull);
        return ResultError;
    }

    // Abort if file system error occurred
    if (result != SdCardResultOk) {
        ErrorCallback(SdCardLoggerErrorFileSystemError);
        return ResultError;
    }
    return ResultOk;
}

/**
 * @brief Closes the file.
 * @return Result.
 */
static Result Close(void) {
    EventCallback(SdCardLoggerEventClose); // may be used to write trailer
    if (SdCardFileClose() != SdCardResultOk) {
        ErrorCallback(SdCardLoggerErrorFileSystemError);
        return ResultError;
    }
    return ResultOk;
}

/**
 * @brief Closes the file then opens a new file.
 * @return Result.
 */
static Result CloseThenOpen(void) {
    const Result result = Close();
    if (result != ResultOk) {
        return result;
    }
    return Open();
}

/**
 * @brief Starts logging.
 */
void SdCardLoggerStart(void) {
    switch (state) {
        case StateDisabled:
            EventCallback(SdCardLoggerEventStart);
            FifoClear(&fifo);
            state = StateOpen;
            break;
        case StateOpen:
        case StateWriting:
            break;
    }
}

/**
 * @brief Stops logging.
 */
void SdCardLoggerStop(void) {
    switch (state) {
        case StateDisabled:
            break;
        case StateOpen:
            EventCallback(SdCardLoggerEventStop);
            state = StateDisabled;
            break;
        case StateWriting:
            EventCallback(SdCardLoggerEventStop);
            state = StateDisabled;
            if ((Write() != ResultOk) || (Write() != ResultOk)) { // write twice to handle FIFO wraparound
                break;
            }
            Close();
            break;
    }
}

/**
 * @brief Returns true if logging.
 * @return True if logging.
 */
bool SdCardLoggerLogging(void) {
    switch (state) {
        case StateDisabled:
            return false;
        case StateOpen:
        case StateWriting:
            return true;
    }
    return false; // avoid compiler warning
}

/**
 * @brief Returns the file name of the current file.
 * @return File name of the current file.
 */
const char* SdCardLoggerGetFileName(void) {
    return fileName;
}

/**
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t SdCardLoggerAvailableWrite(void) {
    return FifoAvailableWrite(&fifo);
}

/**
 * @brief Writes data to the write buffer.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @return Result.
 */
FifoResult SdCardLoggerWrite(const void* const data, const size_t numberOfBytes) {
    return FifoWrite(&fifo, data, numberOfBytes);
}

/**
 * @brief Calls the event callback.
 * @param event Event.
 */
static void EventCallback(const SdCardLoggerEvent event) {
#ifdef PRINT_STATISTICS
    printf("%s\n", SdCardLoggerEventToString(event));
#endif
    if (callbacks.event != NULL) {
        callbacks.event(event);
    }
}

/**
 * @brief Calls the error callback.
 * @param error Error.
 */
static void ErrorCallback(const SdCardLoggerError error) {
#ifdef PRINT_STATISTICS
    printf("%s\n", SdCardLoggerErrorToString(error));
#endif
    if (callbacks.error != NULL) {
        callbacks.error(error);
    }
}

/**
 * @brief Returns a string representation of the event.
 * @param event Event.
 * @return String representation of the event.
 */
const char* SdCardLoggerEventToString(const SdCardLoggerEvent event) {
    switch (event) {
        case SdCardLoggerEventStart:
            return "Start";
        case SdCardLoggerEventStop:
            return "Stop";
        case SdCardLoggerEventOpen:
            return "Open";
        case SdCardLoggerEventMaxFileSizeExceeded:
            return "Max file size exceeded";
        case SdCardLoggerEventMaxFilePeriodExceeded:
            return "Max file period exceeded";
        case SdCardLoggerEventClose:
            return "Close";
    }
    return ""; // avoid compiler warning
}

/**
 * @brief Returns a string representation of the error.
 * @param error Error.
 * @return String representation of the error.
 */
const char* SdCardLoggerErrorToString(const SdCardLoggerError error) {
    switch (error) {
        case SdCardLoggerErrorFileAlreadyExists:
            return "File already exists";
        case SdCardLoggerErrorSdCardFull:
            return "SD card full";
        case SdCardLoggerErrorFileSystemError:
            return "File system error";
    }
    return ""; // avoid compiler warning
}

//------------------------------------------------------------------------------
// End of file
