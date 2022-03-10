/**
 * @file DataLogger.h
 * @author Seb Madgwick
 * @brief Data logger.
 */

#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Data logger settings.
 */
typedef struct {
    char fileNamePrefix[32];
    bool fileNameTimeEnabled;
    bool fileNameCounterEnabled;
    char fileExtension[8];
    size_t maxFileSize; // megabytes (0 = FAT32 limit)
    uint64_t maxFilePeriod; // seconds (0 = unlimited)
} DataLoggerSettings;

/**
 * @brief Data logger status.
 */
typedef enum {
    DataLoggerStatusStart,
    DataLoggerStatusStop,
    DataLoggerStatusOpen,
    DataLoggerStatusNoFileNamesAvailable,
    DataLoggerStatusSDCardFull,
    DataLoggerStatusMaxFileSizeExceeded,
    DataLoggerStatusMaxFilePeriodExceeded,
    DataLoggerStatusSDCardOrFileFull,
    DataLoggerStatusSDCardError,
    DataLoggerStatusClose,
} DataLoggerStatus;

/**
 * @brief Data logger callback functions.
 */
typedef struct {
    void (*statusUpdate)(const DataLoggerStatus status);
    void (*writePreamble)();
} DataLoggerCallbacks;

//------------------------------------------------------------------------------
// Function declarations

void DataLoggerSetSettings(const DataLoggerSettings * const settings_);
void DataLoggerSetCallbacks(const DataLoggerCallbacks * const callbacks_);
void DataLoggerTasks();
void DataLoggerStart();
void DataLoggerStop();
size_t DataLoggerGetWriteAvailable();
void DataLoggerWrite(const void* const data, const size_t numberOfBytes);
const char* DataLoggerGetFileName();
const char* DataLoggerStatusToString(const DataLoggerStatus status);

#endif

//------------------------------------------------------------------------------
// End of file
