/**
 * @file SDCard.h
 * @author Seb Madgwick
 * @brief Application interface for SD card functionality using MPLAB Harmony.
 */

#ifndef SD_CARD_H
#define SD_CARD_H

//------------------------------------------------------------------------------
// Includes

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
 * @brief SD card file time.
 */
typedef struct {
    int second;
    int minute;
    int hour;
    int day;
    int month;
    int year;
} SDCardFileTime;

/**
 * @brief SD card file details.
 */
typedef struct {
    size_t size;
    SDCardFileTime time;
    char name[SD_CARD_MAX_PATH_SIZE];
} SDCardFileDetails;

//------------------------------------------------------------------------------
// Function declarations

void SDCardTasks();
void SDCardMount();
void SDCardUnmount();
bool SDCardIsMounted();
void SDCardFormat();
void SDCardSetVolumeLabel(const char* const label);
SDCardError SDCardFileOpen(const char* const filePath, const bool write);
size_t SDCardFileRead(void* const destination, const size_t destinationSize);
void SDCardFileReadString(void* const destination, const size_t destinationSize);
SDCardError SDCardFileWrite(const void* const data, const size_t numberOfBytes);
SDCardError SDCardFileWriteString(const char* const string);
size_t SDCardFileGetSize();
void SDCardFileClose();
void SDCardRename(const char* const path, const char* const newPath);
void SDCardDelete(const char* const path);
void SDCardDirectoryOpen(const char* const directory);
void SDCardDirectorySearch(const char* const fileName, SDCardFileDetails * const fileDetails);
bool SDCardDirectoryExists(const char* const fileName);
void SDCardDirectoryClose();
const char* SDCardPathSplitFileName(const char* const filePath);
const char* SDCardPathSplitDirectory(const char* const filePath);
const char* SDCardPathJoin(const int numberOfParts, ...);

#endif

//------------------------------------------------------------------------------
// End of file
