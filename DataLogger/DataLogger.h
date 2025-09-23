/**
 * @file DataLogger.h
 * @author Seb Madgwick
 * @brief Data logger.
 */

#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief File name suffix.
 */
typedef enum {
    DataLoggerSuffixNone,
    DataLoggerSuffixCounter,
    DataLoggerSuffixTime,
} DataLoggerSuffix;

/**
 * @brief Settings.
 */
typedef struct {
    char directory[32];
    char fileNamePrefix[32];
    DataLoggerSuffix fileNameSuffix;
    char fileExtension[8];
    size_t maxFileSize; // megabytes (0 = FAT32 limit)
    uint64_t maxFilePeriod; // seconds (0 = unlimited)
} DataLoggerSettings;

/**
 * @brief Status.
 */
typedef enum {
    DataLoggerStatusStart,
    DataLoggerStatusStop,
    DataLoggerStatusOpen,
    DataLoggerStatusMaxFileSizeExceeded,
    DataLoggerStatusMaxFilePeriodExceeded,
    DataLoggerStatusSdCardOrFileFull,
    DataLoggerStatusClose,
} DataLoggerStatus;

/**
 * @brief Error.
 */
typedef enum {
    DataLoggerErrorFileNameUnavailable,
    DataLoggerErrorSdCardFull,
    DataLoggerErrorFileSystemError,
} DataLoggerError;

/**
 * @brief Callbacks. NULL if unused.
 */
typedef struct {
    void (*writePreamble)(void);
    void (*status)(const DataLoggerStatus status);
    void (*error)(const DataLoggerError error);
} DataLoggerCallbacks;

//------------------------------------------------------------------------------
// Function declarations

void DataLoggerSetSettings(const DataLoggerSettings * const settings_);
void DataLoggerSetCallbacks(const DataLoggerCallbacks * const callbacks_);
void DataLoggerTasks(void);
void DataLoggerStart(void);
void DataLoggerStop(void);
bool DataLoggerEnabled(void);
size_t DataLoggerAvailableWrite(void);
FifoResult DataLoggerWrite(const void* const data, const size_t numberOfBytes);
const char* DataLoggerGetFileName(void);
const char* DataLoggerStatusToString(const DataLoggerStatus status);
const char* DataLoggerErrorToString(const DataLoggerError error);

#endif

//------------------------------------------------------------------------------
// End of file
