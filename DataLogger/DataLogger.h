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
    uint32_t fileNameNumber;
    bool fileNameIsTime;
    char fileExtension[16];
    size_t maximumFileSize; // maximum file size in bytes (0 = maximum)
    uint64_t maximumFilePeriod; // maximum file period in timer ticks (0 = infinite)
} DataLoggerSettings;

/**
 * @brief Data logger callback functions.
 */
typedef struct {
    void (*writePreamble)();
    void (*fileNameNumberChanged)(const uint32_t fileNameNumber);
} DataLoggerCallbacks;

/**
 * @brief Data logger status.
 */
typedef enum {
    DataLoggerStatusDisabled,
    DataLoggerStatusEnabled,
    DataLoggerStatusError,
} DataLoggerStatus;

//------------------------------------------------------------------------------
// Function declarations

void DataLoggerSetSettings(const DataLoggerSettings * const settings_);
void DataLoggerSetCallbacks(const DataLoggerCallbacks * const callbacks_);
void DataLoggerStart();
void DataLoggerStop();
DataLoggerStatus DataLoggerGetSatus();
void DataLoggerTasks();
size_t DataLoggerGetWriteAvailable();
void DataLoggerWrite(const void* const data, const size_t numberOfBytes);

#endif

//------------------------------------------------------------------------------
// End of file