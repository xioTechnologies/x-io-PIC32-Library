/**
 * @file SDCard.h
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
    SDCardErrorOK,
    SDCardErrorFileSystemError,
    SDCardErrorFileOrSDCardFull,
} SDCardError;

/**
 * @brief SD card file details.
 */
typedef struct {
    size_t size;
    RtcTime time;
    char name[SD_CARD_MAX_PATH_SIZE];
} SDCardFileDetails;

//------------------------------------------------------------------------------
// Function declarations

void SDCardTasks(void);
void SDCardMount(void);
void SDCardUnmount(void);
bool SDCardIsMounted(void);
void SDCardFormat(void);
void SDCardSetVolumeLabel(const char* const label);
SDCardError SDCardFileOpen(const char* const filePath, const bool write);
size_t SDCardFileRead(void* const destination, const size_t destinationSize);
void SDCardFileReadString(void* const destination, const size_t destinationSize);
SDCardError SDCardFileWrite(const void* const data, const size_t numberOfBytes);
SDCardError SDCardFileWriteString(const char* const string);
size_t SDCardFileGetSize(void);
void SDCardFileClose(void);
void SDCardDirectoryOpen(const char* const directory);
void SDCardDirectorySearch(const char* const fileName, SDCardFileDetails * const fileDetails);
bool SDCardDirectoryExists(const char* const fileName);
void SDCardDirectoryClose(void);
void SDCardPrintDirectory(const char* const directory);
const char* SDCardSizeToString(const size_t size);
void SDCardRename(const char* const path, const char* const newPath);
void SDCardDelete(const char* const path);
const char* SDCardPathSplitFileName(const char* const filePath);
const char* SDCardPathSplitDirectory(const char* const filePath);
const char* SDCardPathJoin(const int numberOfParts, ...);

#endif

//------------------------------------------------------------------------------
// End of file
