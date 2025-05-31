/**
 * @file SdCard.h
 * @author Seb Madgwick
 * @brief Application interface for SD card functionality using MPLAB Harmony.
 */

#ifndef SD_CARD_H
#define SD_CARD_H

//------------------------------------------------------------------------------
// Includes

#include "RtcWeak/RtcWeak.h"
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief SD card maximum path size.
 */
#define SD_CARD_MAX_PATH_SIZE (256)

/**
 * @brief SD card error.
 */
typedef enum {
    SdCardErrorOk,
    SdCardErrorFileSystemError,
    SdCardErrorFileOrSdCardFull,
} SdCardError;

/**
 * @brief SD card file details.
 */
typedef struct {
    size_t size;
    RtcTime time;
    char name[SD_CARD_MAX_PATH_SIZE];
} SdCardFileDetails;

//------------------------------------------------------------------------------
// Function declarations

void SdCardTasks(void);
void SdCardMount(void);
void SdCardUnmount(void);
bool SdCardMounted(void);
void SdCardFormat(void);
void SdCardSetVolumeLabel(const char* const label);
SdCardError SdCardFileOpen(const char* const filePath, const bool write);
size_t SdCardFileRead(void* const destination, const size_t destinationSize);
void SdCardFileReadString(void* const destination, const size_t destinationSize);
SdCardError SdCardFileWrite(const void* const data, const size_t numberOfBytes);
SdCardError SdCardFileWriteString(const char* const string);
size_t SdCardFileGetSize(void);
void SdCardFileClose(void);
void SdCardDirectoryOpen(const char* const directory);
void SdCardDirectorySearch(const char* const fileName, SdCardFileDetails * const fileDetails);
bool SdCardDirectoryExists(const char* const fileName);
void SdCardDirectoryClose(void);
void SdCardPrintDirectory(const char* const directory);
const char* SdCardSizeToString(const size_t size);
void SdCardRename(const char* const path, const char* const newPath);
void SdCardDelete(const char* const path);
const char* SdCardPathSplitFileName(const char* const filePath);
const char* SdCardPathSplitDirectory(const char* const filePath);
const char* SdCardPathJoin(const int numberOfParts, ...);

#endif

//------------------------------------------------------------------------------
// End of file
