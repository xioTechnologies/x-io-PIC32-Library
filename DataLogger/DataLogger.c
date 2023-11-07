/**
 * @file DataLogger.c
 * @author Seb Madgwick
 * @brief Data logger.
 */

//------------------------------------------------------------------------------
// Includes

#include "CircularBuffer.h"
#include <ctype.h> // isalnum
#include "DataLogger.h"
#include "definitions.h"
#include "Rtc/Rtc.h"
#include "SDCard/SDCard.h"
#include <stdbool.h>
#include <stdio.h> // printf, snprintf, sscanf
#include <string.h> // strcpy, strlen
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Directory.
 */
#define DIRECTORY "Data Logger"

/**
 * @brief Counter file path.
 */
#define COUNTER_FILE_PATH (DIRECTORY "/Counter.txt")

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
} State;

//------------------------------------------------------------------------------
// Function declarations

static void StateOpenTasks(void);
static void StateWriteTasks(void);
static int Open(void);
static uint32_t ReadCounter(void);
static void WriteCounter(const uint32_t counter);
static int Write(void);
static void Close(void);
static void UpdateStatus(const DataLoggerStatus status);
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
 * @brief Module tasks.  This function should be called repeatedly within the
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
    if (SDCardIsMounted() == false) {
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

    // Create time string
    char timeString[32] = "";
    if (settings.fileNameTimeEnabled == true) {
        RtcTime time;
        RtcGetTime(&time);
        snprintf(timeString, sizeof (timeString), "_%04u-%02u-%02u_%02u-%02u-%02u", time.year, time.month, time.day, time.hour, time.minute, time.second);
    }

    // Open directory
    SDCardDirectoryOpen(DIRECTORY);

    // Create file name without counter
    if (settings.fileNameCounterEnabled == false) {
        snprintf(fileName, sizeof (fileName), "%s%s%s", settings.fileNamePrefix, timeString, settings.fileExtension);
        if (SDCardDirectoryExists(fileName) == true) {
            strcpy(fileName, "");
        }
    }

    // Create file name with counter
    if ((settings.fileNameCounterEnabled == true) || (strlen(fileName) == 0)) {
        const uint32_t initialCounter = ReadCounter();
        uint32_t counter = initialCounter;
        while (true) {
            snprintf(fileName, sizeof (fileName), "%s%s_%04u%s", settings.fileNamePrefix, timeString, counter, settings.fileExtension);
            if (++counter > 9999) {
                counter = 0;
            }
            if (SDCardDirectoryExists(fileName) == false) {
                break;
            }
            if (counter == initialCounter) {
                strcpy(fileName, "");
                break;
            }
        }
        WriteCounter(counter);
    }

    // Close directory
    SDCardDirectoryClose();

    // Remove invalid characters
    int readIndex = 0;
    int writeIndex = 0;
    while (fileName[readIndex] != '\0') {
        const char character = fileName[readIndex++];
        if ((isalnum(character) == true) || (character == '-') || (character == '.') || (character == '_')) {
            fileName[writeIndex++] = character;
        }
    }
    fileName[writeIndex] = '\0';

    // Abort if no file names avaliable
    if (strlen(fileName) == 0) {
        UpdateStatus(DataLoggerStatusNoFileNamesAvailable);
        return 1;
    }

    // Open file
    switch (SDCardFileOpen(SDCardPathJoin(2, DIRECTORY, fileName), true)) {
        case SDCardErrorOK:
            break;
        case SDCardErrorFileSystemError:
            UpdateStatus(DataLoggerStatusFileSystemError);
            return 1;
        case SDCardErrorFileOrSDCardFull:
            UpdateStatus(DataLoggerStatusSDCardFull);
            return 1;
    }
    UpdateStatus(DataLoggerStatusOpen);
#ifdef PRINT_STATISTICS
    printf("%s\n", fileName);
#endif

    // Write preamble
    if (callbacks.writePreamble != NULL) {
        callbacks.writePreamble(); // application use SDCardFileWrite to write preamble
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
static uint32_t ReadCounter(void) {
    if (SDCardFileOpen(COUNTER_FILE_PATH, false) != SDCardErrorOK) {
        return 0;
    }
    char string[8];
    const size_t numberOfBytes = SDCardFileRead(string, sizeof (string));
    SDCardFileClose();
    if (numberOfBytes == -1) {
        return 0;
    }
    uint32_t counter;
    if (sscanf(string, "%u", &counter) != 1) {
        return 0;
    }
    return counter;
}

/**
 * @brief Writes the counter to file.
 * @param Counter.
 */
static void WriteCounter(const uint32_t counter) {
    if (SDCardFileOpen(COUNTER_FILE_PATH, true) != SDCardErrorOK) {
        return;
    }
    char string[8];
    snprintf(string, sizeof (string), "%u", counter);
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
            UpdateStatus(DataLoggerStatusMaxFilePeriodExceeded);
            Close();
            return Open();
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
            UpdateStatus(DataLoggerStatusMaxFileSizeExceeded);
            Close();
            return Open();
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
        UpdateStatus(DataLoggerStatusSDCardOrFileFull);
        Close();
        return Open();
    }

    // Abort if error occurred
    if (sdCardError != SDCardErrorOK) {
        UpdateStatus(DataLoggerStatusFileSystemError);
        return 1;
    }
    return 0;
}

/**
 * @brief Closes the file.
 */
static void Close(void) {
    UpdateStatus(DataLoggerStatusClose);
    SDCardFileClose();
}

/**
 * @brief Starts logging.
 */
void DataLoggerStart(void) {
    UpdateStatus(DataLoggerStatusStart);
    switch (state) {
        case StateDisabled:
            break;
        case StateOpen:
        case StateWrite:
            return;
    }
    CircularBufferClear(&buffer);
    state = StateOpen;
}

/**
 * @brief Stops logging.
 */
void DataLoggerStop(void) {
    UpdateStatus(DataLoggerStatusStop);
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
 * @brief Returns the space available in the write buffer.
 * @return Space available in the write buffer.
 */
size_t DataLoggerGetWriteAvailable(void) {
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

/**
 * @brief Returns the file name of the current file.
 * @return File name of the current file.
 */
const char* DataLoggerGetFileName(void) {
    return fileName;
}

/**
 * @brief Updates the status.
 * @param status Status.
 */
static void UpdateStatus(const DataLoggerStatus status) {
#ifdef PRINT_STATISTICS
    printf("%s\n", DataLoggerStatusToString(status));
#endif
    if (callbacks.statusUpdate != NULL) {
        callbacks.statusUpdate(status);
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
        case DataLoggerStatusNoFileNamesAvailable:
            return "No file names avaliable";
        case DataLoggerStatusSDCardFull:
            return "SD card full";
        case DataLoggerStatusMaxFileSizeExceeded:
            return "Max file size exceeded";
        case DataLoggerStatusMaxFilePeriodExceeded:
            return "Max file period exceeded";
        case DataLoggerStatusSDCardOrFileFull:
            return "SD card or file full";
        case DataLoggerStatusFileSystemError:
            return "File system error";
        case DataLoggerStatusClose:
            return "Close";
    }
    return "Unknown status";
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
    snprintf(bufferUsageString, sizeof (bufferUsageString), "%0.1f%%", ((float) maxbufferUsed * (100.0f / (float) sizeof (bufferData))));

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
