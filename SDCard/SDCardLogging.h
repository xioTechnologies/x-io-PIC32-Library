/**
 * @file SDCardLogging.h
 * @author Seb Madgwick
 * @brief Application interface for continuous logging of data to an SD card.
 */

#ifndef SD_CARD_LOGGING_H
#define SD_CARD_LOGGING_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief SD card logging settings.
 */
typedef struct {
    char fileNamePrefix[32];
    uint32_t fileNameNumber;
    bool fileNameIsTime;
    char fileExtension[16];
    size_t maximumFileSize; // maximum file size in bytes (0 = maximum)
    uint64_t maximumFilePeriod; // maximum file period in timer ticks (0 = infinite)
} SDCardLoggingSettings;

/**
 * @brief SD card logging callback functions.
 */
typedef struct {
    void (*writePreamble)();
    void (*fileNameNumberChanged)(const uint32_t fileNameNumber);
} SDCardLoggingCallbacks;

/**
 * @brief SD card logging status.
 */
typedef enum {
    SDCardLoggingStatusDisabled,
    SDCardLoggingStatusEnabled,
    SDCardLoggingStatusError,
} SDCardLoggingStatus;

//------------------------------------------------------------------------------
// Function declarations

void SDCardLoggingSetSettings(const SDCardLoggingSettings * const settings_);
void SDCardLoggingSetCallbacks(const SDCardLoggingCallbacks * const callbacks_);
void SDCardLoggingStart();
void SDCardLoggingStop();
SDCardLoggingStatus SDCardLoggingGetSatus();
void SDCardLoggingTasks();
size_t SDCardLoggingGetWriteAvailable();
void SDCardLoggingWrite(const void* const data, const size_t numberOfBytes);

#endif

//------------------------------------------------------------------------------
// End of file