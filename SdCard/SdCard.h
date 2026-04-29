/**
 * @file SdCard.h
 * @author Seb Madgwick
 * @brief Wrapper for the MPLAB Harmony SD card driver.
 */

#ifndef SD_CARD_H
#define SD_CARD_H

//------------------------------------------------------------------------------
// Includes

#include "RtcWeak/RtcWeak.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Maximum path size.
 */
#define SD_CARD_MAX_PATH_SIZE (256)

/**
 * @brief FAT32 maximum file size.
 */
#define SD_CARD_MAX_FILE_SIZE (UINT32_MAX - 1)

/**
 * @brief Capacity.
 */
typedef struct {
    uint64_t capacity;
    uint64_t used;
    uint64_t available;
} SdCardCapacity;

/**
 * @brief File status.
 */
typedef struct {
    size_t size;
    RtcTime time;
    bool isDirectory;
    char name[SD_CARD_MAX_PATH_SIZE];
} SdCardFileStatus;

/**
 * @brief Result.
 */
typedef enum {
    SdCardResultOk,
    SdCardResultFileOrSdCardFull, /* only returned by SdCardFileWrite and SdCardFileWriteString */
    SdCardResultFileSystemError,
} SdCardResult;

//------------------------------------------------------------------------------
// Function declarations

void SdCardTasks(void);
void SdCardMount(void);
void SdCardUnmount(void);
bool SdCardMounted(void);
SdCardResult SdCardFormat(void);
SdCardResult SdCardGetCapacity(SdCardCapacity * const capacity);
SdCardResult SdCardSetVolumeLabel(const char* const label);
SdCardResult SdCardFileOpen(const char* const filePath);
SdCardResult SdCardFileCreate(const char* const filePath);
SdCardResult SdCardFileRead(void* const destination, const size_t destinationSize, size_t * const numberOfBytes);
SdCardResult SdCardFileReadString(void* const destination, const size_t destinationSize);
SdCardResult SdCardFileWrite(const void* const data, const size_t numberOfBytes);
SdCardResult SdCardFileWriteString(const char* const string);
SdCardResult SdCardFileGetSize(size_t * const size);
SdCardResult SdCardFileFlush(void);
SdCardResult SdCardFileClose(void);
SdCardResult SdCardFileQuickRead(const char* const filePath, void* const destination, const size_t destinationSize, size_t * const numberOfBytes);
SdCardResult SdCardFileQuickReadString(const char* const filePath, void* const destination, const size_t destinationSize);
SdCardResult SdCardFileQuickWrite(const char* const filePath, const void* const data, const size_t numberOfBytes);
SdCardResult SdCardFileQuickWriteString(const char* const filePath, const char* const string);
SdCardResult SdCardDirectoryOpen(const char* const directory);
SdCardResult SdCardDirectorySearch(const char* const fileName, SdCardFileStatus * const fileStatus);
bool SdCardDirectoryExists(const char* const fileName);
SdCardResult SdCardDirectoryClose(void);
SdCardResult SdCardDirectoryQuickSearch(const char* const directory, const char* const fileName, SdCardFileStatus * const fileStatus);
bool SdCardDirectoryQuickExists(const char* const directory, const char* const fileName);
SdCardResult SdCardRename(const char* const oldPath, const char* const newPath);
SdCardResult SdCardDelete(const char* const path);
const char* SdCardPathFileName(const char* const filePath);
const char* SdCardPathExtension(const char* const filePath);
void SdCardPathStem(char* const destination, const size_t destinationSize, const char* const filePath);
void SdCardPathDirectory(char* const destination, const size_t destinationSize, const char* const filePath);
void SdCardPathJoin(char* const destination, const size_t destinationSize, const int numberOfParts, ...);
void SdCardPathAppend(char* const destination, const size_t destinationSize, const char* const part);
void SdCardPathSetExtension(char* const destination, const size_t destinationSize, const char* const extension);
void SdCardSizeToString(char* const destination, const size_t destinationSize, const uint64_t size);
const char* SdCardResultToString(const SdCardResult result);
void SdCardPrintCapacity(void);
void SdCardPrintDirectory(const char* const directory);
const char* SdCardGetFileSystemError(void);

#endif

//------------------------------------------------------------------------------
// End of file
