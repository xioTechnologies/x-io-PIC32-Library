/**
 * @file SdCardLogger.h
 * @author Seb Madgwick
 * @brief SD card logger.
 */

#ifndef SD_CARD_LOGGER_H
#define SD_CARD_LOGGER_H

//------------------------------------------------------------------------------
// Includes

#include "Fifo.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief File name prefix size.
 */
#define SD_CARD_LOGGER_FILE_NAME_PREFIX_SIZE (64)

/**
 * @brief File name extension size.
 */
#define SD_CARD_LOGGER_FILE_EXTENSION_SIZE (8)

/**
 * @brief File name suffix.
 */
typedef enum {
    SdCardLoggerSuffixNone,
    SdCardLoggerSuffixCounter,
    SdCardLoggerSuffixDateAndTime,
} SdCardLoggerSuffix;

/**
 * @brief Settings.
 */
typedef struct {
    char fileNamePrefix[SD_CARD_LOGGER_FILE_NAME_PREFIX_SIZE];
    SdCardLoggerSuffix fileNameSuffix;
    char fileExtension[SD_CARD_LOGGER_FILE_EXTENSION_SIZE];
    size_t maxFileSize; // 0 = FAT32 limit
    uint32_t maxFilePeriod; // seconds (0 = unlimited)
} SdCardLoggerSettings;

/**
 * @brief Event.
 */
typedef enum {
    SdCardLoggerEventStart,
    SdCardLoggerEventStop,
    SdCardLoggerEventOpen, /* use SdCardFileWrite to write preamble */
    SdCardLoggerEventMaxFileSizeExceeded,
    SdCardLoggerEventMaxFilePeriodExceeded,
    SdCardLoggerEventClose, /* use SdCardFileWrite to write trailer */
} SdCardLoggerEvent;

/**
 * @brief Error.
 */
typedef enum {
    SdCardLoggerErrorFileAlreadyExists,
    SdCardLoggerErrorSdCardFull,
    SdCardLoggerErrorFileSystemError,
} SdCardLoggerError;

/**
 * @brief Callbacks. NULL if unused.
 */
typedef struct {
    void (*event)(const SdCardLoggerEvent event);
    void (*error)(const SdCardLoggerError error);
} SdCardLoggerCallbacks;

//------------------------------------------------------------------------------
// Function declarations

void SdCardLoggerSetSettings(const SdCardLoggerSettings * const settings_);
void SdCardLoggerSetCallbacks(const SdCardLoggerCallbacks * const callbacks_);
void SdCardLoggerTasks(void);
void SdCardLoggerStart(void);
void SdCardLoggerStop(void);
bool SdCardLoggerLogging(void);
const char* SdCardLoggerGetFileName(void);
size_t SdCardLoggerAvailableWrite(void);
FifoResult SdCardLoggerWrite(const void* const data, const size_t numberOfBytes);
const char* SdCardLoggerEventToString(const SdCardLoggerEvent event);
const char* SdCardLoggerErrorToString(const SdCardLoggerError error);

#endif

//------------------------------------------------------------------------------
// End of file
